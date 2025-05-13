import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import os

# === Adaptive Biquad Class ===
class AdaptiveBiquad:
    def __init__(self, sample_rate=44100, min_gain_db=-60, max_gain_db=-10, alpha=0.5):
        self.sample_rate = sample_rate
        self.min_biquad = None
        self.max_biquad = None
        self.min_gain_db = min_gain_db
        self.max_gain_db = max_gain_db
        self.alpha = alpha
        self.current_scale = 0.0  

    def set_biquads(self, min_biquad, max_biquad):
        """Set min and max biquad coefficients."""
        self.min_biquad = min_biquad
        self.max_biquad = max_biquad

    def interpolate_gain(self, input_gain_db):
        """Compute the smoothed gain ratio."""
        if input_gain_db >= self.max_gain_db:
            raw_scale = 1.0
        elif input_gain_db <= self.min_gain_db:
            raw_scale = 0.0
        else:
            raw_scale = (input_gain_db - self.min_gain_db) / (self.max_gain_db - self.min_gain_db)

        self.current_scale = self.alpha * raw_scale + (1 - self.alpha) * self.current_scale  
        return self.current_scale

    def interpolate_coeffs(self):
        """Interpolate between min and max biquad coefficients."""
        if self.min_biquad is None or self.max_biquad is None:
            raise ValueError("Biquad coefficients not set")

        interp_b = (1 - self.current_scale) * self.min_biquad[0] + self.current_scale * self.max_biquad[0]
        interp_a = (1 - self.current_scale) * self.min_biquad[1] + self.current_scale * self.max_biquad[1]
        return interp_b, interp_a


# === Configuration ===
config = {
    "sample_rate": 44100,
    "frames": 100,
    "min_gain_db": -60,
    "max_gain_db": -10,
    "alpha": 0.5,
    "min_eq": {"filter_type": "peaking", "freq": 1000, "Q": 0.5, "gain_db": 3},
    "max_eq": {"filter_type": "lowshelf", "freq": 100, "Q": 0.7, "gain_db": -4},
}


# === Biquad Coefficient Calculation ===
def calculate_biquad(filter_type, freq, Q, gain_db=0, sample_rate=44100):
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
    else:
        raise ValueError("Unsupported filter type")

    return np.array([b0, b1, b2]) / a0, np.array([1, a1 / a0, a2 / a0])


# === Initialize Adaptive Biquad ===
biquad = AdaptiveBiquad(config["sample_rate"], config["min_gain_db"], config["max_gain_db"], config["alpha"])

# Compute and Set Biquads
def set_biquads_from_config(biquad, config):
    min_b, min_a = calculate_biquad(**config["min_eq"], sample_rate=config["sample_rate"])
    max_b, max_a = calculate_biquad(**config["max_eq"], sample_rate=config["sample_rate"])
    biquad.set_biquads((min_b, min_a), (max_b, max_a))

set_biquads_from_config(biquad, config)


# === Plot Setup ===
fig, ax = plt.subplots(figsize=(10, 5))
ax.set_xscale("log")
ax.set_xlabel("Frequency (Hz)")
ax.set_ylabel("Gain (dB)")
ax.set_title("EQ Interpolation using Adaptive Biquad")
ax.grid(which="both", linestyle="--", linewidth=0.5)
ax.axhline(0, color="black", linewidth=0.8)
ax.set_xlim(20, config["sample_rate"] / 2)
ax.set_ylim(-10, 10)

w, h_min = signal.freqz(*biquad.min_biquad, worN=1024, fs=config["sample_rate"])
_, h_max = signal.freqz(*biquad.max_biquad, worN=1024, fs=config["sample_rate"])

h_min_db = 20 * np.log10(abs(h_min))
h_max_db = 20 * np.log10(abs(h_max))

ax.plot(w, h_min_db, label="Min EQ", color="red", linestyle="dashed")
ax.plot(w, h_max_db, label="Max EQ", color="blue", linestyle="dashed")

line, = ax.plot(w, h_min_db, label="Interpolated EQ", color="black", linewidth=2)
slider_label = ax.text(100, -9, "Input Gain: -40 dB", fontsize=12, bbox=dict(facecolor='white', edgecolor='black'))
ax.legend()


# === Animation Function ===
def animate_eq_curve(i):
    # Ensure smooth oscillation between min_gain_db - 10 and max_gain_db + 10
    input_gain_db = ((1 - np.cos(2 * np.pi * i / config["frames"])) / 2) * (
        (config["max_gain_db"] + 10) - (config["min_gain_db"] - 10)
    ) + (config["min_gain_db"] - 10)

    gain_ratio = biquad.interpolate_gain(input_gain_db)
    interp_b, interp_a = biquad.interpolate_coeffs()

    w, h_interp = signal.freqz(interp_b, interp_a, worN=1024, fs=config["sample_rate"])
    h_interp_db = 20 * np.log10(abs(h_interp))

    line.set_ydata(h_interp_db)
    slider_label.set_text(f"Input Gain: {input_gain_db:.1f} dB")
    return line, slider_label

# === Animation ===
plot_dir = "Plots"
os.makedirs(plot_dir, exist_ok=True)  
animation_path = os.path.join(plot_dir, "adaptive_biquad.gif")

ani = animation.FuncAnimation(fig, animate_eq_curve, frames=config["frames"], interval=50, blit=True)
ani.save(animation_path, writer="pillow", fps=15)

print(f"Animation saved to {animation_path}")
