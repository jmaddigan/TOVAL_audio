import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import os

def biquad_coefficients(filter_type, freq, Q, gain_db=0, sample_rate=44100):
    """
    Calculates biquad filter coefficients for a given filter type and parameters.
    """
    A = 10**(gain_db / 40)  # Gain factor for shelving/peaking filters
    omega = 2 * np.pi * freq / sample_rate
    alpha = np.sin(omega) / (2 * Q)

    if filter_type == "lowpass":
        b0 = (1 - np.cos(omega)) / 2
        b1 = 1 - np.cos(omega)
        b2 = (1 - np.cos(omega)) / 2
        a0 = 1 + alpha
        a1 = -2 * np.cos(omega)
        a2 = 1 - alpha

    elif filter_type == "highpass":
        b0 = (1 + np.cos(omega)) / 2
        b1 = -(1 + np.cos(omega))
        b2 = (1 + np.cos(omega)) / 2
        a0 = 1 + alpha
        a1 = -2 * np.cos(omega)
        a2 = 1 - alpha

    elif filter_type == "bandpass":
        b0 = Q * alpha
        b1 = 0
        b2 = -Q * alpha
        a0 = 1 + alpha
        a1 = -2 * np.cos(omega)
        a2 = 1 - alpha

    elif filter_type == "notch":
        b0 = 1
        b1 = -2 * np.cos(omega)
        b2 = 1
        a0 = 1 + alpha
        a1 = -2 * np.cos(omega)
        a2 = 1 - alpha

    elif filter_type == "peaking":
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

    elif filter_type == "highshelf":
        beta = np.sqrt(A) / Q
        b0 = A * ((A + 1) + (A - 1) * np.cos(omega) + beta * np.sin(omega))
        b1 = -2 * A * ((A - 1) + (A + 1) * np.cos(omega))
        b2 = A * ((A + 1) + (A - 1) * np.cos(omega) - beta * np.sin(omega))
        a0 = (A + 1) - (A - 1) * np.cos(omega) + beta * np.sin(omega)
        a1 = 2 * ((A - 1) - (A + 1) * np.cos(omega))
        a2 = (A + 1) - (A - 1) * np.cos(omega) - beta * np.sin(omega)

    # Normalize coefficients
    return np.array([b0, b1, b2]) / a0, np.array([1, a1 / a0, a2 / a0])

def interpolate_biquad(min_coeffs, max_coeffs, gain, min_gain, max_gain):
    """Interpolates between min and max biquad coefficients based on gain."""
    gain_db = 20 * np.log10(gain)  # Convert linear gain to dB
    gain_ratio = np.clip((gain_db - min_gain) / (max_gain - min_gain), 0, 1)
    return (1 - gain_ratio) * min_coeffs + gain_ratio * max_coeffs

def plot_eq_curve(min_eq, max_eq, input_gain, min_gain_db, max_gain_db, sample_rate=44100):
    """
    Plots the EQ curves for the min, max, and interpolated biquad coefficients.
    """
    # Get min and max biquad coefficients
    min_b, min_a = biquad_coefficients(**min_eq)
    max_b, max_a = biquad_coefficients(**max_eq)

    # Interpolate coefficients based on input gain
    interp_b = interpolate_biquad(min_b, max_b, input_gain, min_gain_db, max_gain_db)
    interp_a = interpolate_biquad(min_a, max_a, input_gain, min_gain_db, max_gain_db)

    # Compute frequency response
    w, h_min = signal.freqz(min_b, min_a, worN=1024, fs=sample_rate)
    _, h_max = signal.freqz(max_b, max_a, worN=1024, fs=sample_rate)
    _, h_interp = signal.freqz(interp_b, interp_a, worN=1024, fs=sample_rate)

    # Convert magnitude to dB
    h_min_db = 20 * np.log10(abs(h_min))
    h_max_db = 20 * np.log10(abs(h_max))
    h_interp_db = 20 * np.log10(abs(h_interp))

    # Plot results
    plt.figure(figsize=(10, 5))
    plt.plot(w, h_min_db, label="Min EQ", color="red", linestyle="dashed")
    plt.plot(w, h_max_db, label="Max EQ", color="blue", linestyle="dashed")
    plt.plot(w, h_interp_db, label="Interpolated EQ", color="black", linewidth=2)
    plt.xscale("log")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Gain (dB)")
    plt.title("Adaptive Biquad EQ Response")
    plt.grid(which="both", linestyle="--", linewidth=0.5)
    plt.axhline(0, color="black", linewidth=0.8)
    plt.legend()

    # Save the plot
    plot_dir = "Plots"
    os.makedirs(plot_dir, exist_ok=True)
    plot_path = os.path.join(plot_dir, "adaptive_eq_curve.png")
    plt.savefig(plot_path)
    plt.show()

# Example usage
min_eq_settings = {
    "filter_type": "peaking",
    "freq": 1000,
    "Q": 1,
    "gain_db": 6
}
max_eq_settings = {
    "filter_type": "lowshelf",
    "freq": 200,
    "Q": 0.7,
    "gain_db": -3
}
input_gain = 0.2  # Linear input gain (example)
min_gain_db = -40  # dB level where min EQ applies
max_gain_db = -10  # dB level where max EQ applies

plot_eq_curve(min_eq_settings, max_eq_settings, input_gain, min_gain_db, max_gain_db)
