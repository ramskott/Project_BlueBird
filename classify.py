#!/usr/bin/env python3
"""
🐦 BlueBird - Simple Audio Classifier
Reads audio files (.mp3 / .wav) and uses your Teachable Machine model
to tell you if it hears a Sparrow, Rain, Human, or Background Noise!
"""

import os
import sys
import glob
import numpy as np
import soundfile as sf
from scipy import signal
from ai_edge_litert.interpreter import Interpreter

# Paths to your model files
MODEL_PATH = "model/soundclassifier_with_metadata.tflite"
LABELS_PATH = "model/labels.txt"
INPUT_FOLDER = "input_audio"

# 1. Load the Labels (e.g., Sparrow Sounds, Rain, Background Noise)
with open(LABELS_PATH, "r") as f:
    LABELS = [line.strip() for line in f if line.strip()]

# 2. Load the AI Model
interpreter = Interpreter(model_path=MODEL_PATH)
interpreter.allocate_tensors()
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

def classify_one_second(audio_1s):
    """Feeds 1 second of sound into the AI model and returns scores."""
    # Ensure correct shape [1, 44032]
    audio_input = np.zeros((1, 44032), dtype=np.float32)
    length = min(len(audio_1s), 44032)
    audio_input[0, :length] = audio_1s[:length]

    # Ask the AI model
    interpreter.set_tensor(input_details[0]['index'], audio_input)
    interpreter.invoke()
    scores = interpreter.get_tensor(output_details[0]['index'])[0]
    return scores

def classify_audio_file(file_path):
    """Loads any .mp3 or .wav file and checks if a sparrow is heard."""
    print(f"\n🎧 Listening to: {os.path.basename(file_path)}")
    
    # Load audio and convert stereo to mono
    audio, sample_rate = sf.read(file_path, dtype='float32')
    if audio.ndim > 1:
        audio = np.mean(audio, axis=1)

    # Resample to 44,100 Hz (what the model expects)
    if sample_rate != 44100:
        num_samples = int(len(audio) * (44100 / sample_rate))
        audio = signal.resample(audio, num_samples).astype(np.float32)

    duration_seconds = len(audio) / 44100
    print(f"⏱️  Duration: {duration_seconds:.1f} seconds")

    # Slide across the audio in 1-second chunks (step by 0.5s)
    step = int(0.5 * 44100)
    window = 44032
    highest_sparrow_score = 0.0
    best_time = 0.0
    sparrow_chirps_count = 0

    for start in range(0, max(1, len(audio) - window + 1), step):
        chunk = audio[start : start + window]
        scores = classify_one_second(chunk)
        
        # Check Sparrow score (Index 2 in labels.txt)
        sparrow_score = float(scores[2])
        if sparrow_score > 0.50:
            sparrow_chirps_count += 1
        if sparrow_score > highest_sparrow_score:
            highest_sparrow_score = sparrow_score
            best_time = start / 44100

    # Print the friendly result
    print("-" * 45)
    if highest_sparrow_score >= 0.50:
        print(f"🎉 SPARROW DETECTED! (Confidence: {highest_sparrow_score * 100:.1f}%)")
        print(f"📍 Loudest chirp heard around: {best_time:.1f} seconds")
        print(f"🔢 Total sparrow calls detected: {sparrow_chirps_count}")
    else:
        print(f"❌ No Sparrow heard (Highest score: {highest_sparrow_score * 100:.1f}%)")
    print("-" * 45)

def main():
    # If a specific file is passed in the terminal, classify that one
    if len(sys.argv) > 1:
        target_file = sys.argv[1]
        classify_audio_file(target_file)
    else:
        # Otherwise, classify all .mp3 and .wav files in the input_audio folder
        audio_files = glob.glob(f"{INPUT_FOLDER}/*.mp3") + glob.glob(f"{INPUT_FOLDER}/*.wav")
        if not audio_files:
            print(f"No audio files found in '{INPUT_FOLDER}'! Please drop some .mp3 or .wav files there.")
            return

        print(f"Found {len(audio_files)} audio file(s) in '{INPUT_FOLDER}/'. Starting classification...\n")
        for file in sorted(audio_files):
            classify_audio_file(file)

if __name__ == "__main__":
    main()
