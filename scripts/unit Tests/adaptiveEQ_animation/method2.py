import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import os

# === Parameters ===
sample_rate = 44100
min_gain_db = -60  
max_gain_db = -10  
frames = 100  
smoothing_alpha = 0.5

# Extend input gain range
min_osc_db = min_gain_db - 10  
max_osc_db = max_gain_db + 10  

# Filter settings
min_eq = {"filter_type": "peaking", "freq": 1000, "Q": 0.5, "gain_db": 3}
max_eq = {"filter_type": "lowshelf", "freq": 100, "Q": 0.7, "gain_db": -4}

def biquad_coefficients(filter_type, freq, Q, gain_db=0, sample_rate=44100):
    A = 10**(gain_db / 40)  
    omega = 2 * np.pi * freq / sample_rate
    alpha = np.sin(omega) / (2 * Q)

    if filter_type == "peaking":
        b0 = 1 + alpha * A
        b1 = -2 * np.cos(omega)
        b2 = 1 - alpha * A
        a0 = 1 + alpha / A
        a1 = -2 * np.cos(omega)
        a2 = 1 - alpha / A
    elif filter_type == "lowshelf":
        beta = np.sqrt(A) / Q
        b0 = A * ((A + 1) - (A - 1) * np.cos(omega) + beta * np.sin(omega))
        b1 = 2 * A * ((A - 1) - (A + 1) * np.cos(omega))
        b2 = A * ((A + 1) - (A - 1) * np.cos(omega) - beta * np.sin(omega))
        a0 = (A + 1) + (A - 1) * np.cos(omega) + beta * np.sin(omega)
        a1 = -2 * ((A - 1) + (A + 1) * np.cos(omega))
        a2 = (A + 1) + (A - 1) * np.cos(omega) - beta * np.sin(omega)
    return np.array([b0, b1, b2]) / a0, np.array([1, a1 / a0, a2 / a0])

def interpolate_biquad(min_coeffs, max_coeffs, gain_ratio):
    return (1 - gain_ratio) * min_coeffs + gain_ratio * max_coeffs

smoothed_gain_ratio = None

def animate_eq_curve(i):
    global smoothed_gain_ratio
    
    input_gain_db = ((1 - np.cos(2 * np.pi * i / frames)) / 2) * (max_osc_db - min_osc_db) + min_osc_db
    gain_ratio = np.clip((input_gain_db - min_gain_db) / (max_gain_db - min_gain_db), 0, 1)
    
    if smoothed_gain_ratio is None:
        smoothed_gain_ratio = gain_ratio  
    else:
        smoothed_gain_ratio = smoothing_alpha * gain_ratio + (1 - smoothing_alpha) * smoothed_gain_ratio
    
    interp_b = interpolate_biquad(min_b, max_b, smoothed_gain_ratio)
    interp_a = interpolate_biquad(min_a, max_a, smoothed_gain_ratio)

    w, h_interp = signal.freqz(interp_b, interp_a, worN=1024, fs=sample_rate)
    h_interp_db = 20 * np.log10(abs(h_interp))
    
    line.set_ydata(h_interp_db)
    slider_label.set_text(f"Input Gain: {input_gain_db:.1f} dB")  
    return line, slider_label

min_b, min_a = biquad_coefficients(**min_eq, sample_rate=sample_rate)
max_b, max_a = biquad_coefficients(**max_eq, sample_rate=sample_rate)

w, h_min = signal.freqz(min_b, min_a, worN=1024, fs=sample_rate)
_, h_max = signal.freqz(max_b, max_a, worN=1024, fs=sample_rate)

h_min_db = 20 * np.log10(abs(h_min))
h_max_db = 20 * np.log10(abs(h_max))

fig, ax = plt.subplots(figsize=(10, 5))
ax.set_xscale("log")
ax.set_xlabel("Frequency (Hz)")
ax.set_ylabel("Gain (dB)")
ax.set_title("EQ Interpolation: Smooth Gain Scaling factor")
ax.grid(which="both", linestyle="--", linewidth=0.5)
ax.axhline(0, color="black", linewidth=0.8)
ax.set_xlim(20, sample_rate / 2)
ax.set_ylim(-10, 10)

ax.plot(w, h_min_db, label="Min EQ", color="red", linestyle="dashed")
ax.plot(w, h_max_db, label="Max EQ", color="blue", linestyle="dashed")

line, = ax.plot(w, h_min_db, label="Interpolated EQ", color="black", linewidth=2)
slider_label = ax.text(100, -9, "Input Gain: -40 dB", fontsize=12, bbox=dict(facecolor='white', edgecolor='black'))
alpha_label = ax.text(30, 8, f"Smoothing Alpha: {smoothing_alpha}", 
                      fontsize=12, bbox=dict(facecolor='white', edgecolor='black'))
ax.legend()

plot_dir = "Plots"
os.makedirs(plot_dir, exist_ok=True)  
animation_path = os.path.join(plot_dir, "method2.gif")

ani = animation.FuncAnimation(fig, animate_eq_curve, frames=frames, interval=50, blit=True)
ani.save(animation_path, writer="pillow", fps=15)

print(f"Animation saved to {animation_path}")
