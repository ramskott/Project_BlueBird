# 🐦 BlueBird: Simple Guide for Beginners

Welcome to **BlueBird**! This guide explains how this project works in plain, simple English.

---

## 🎯 1. What Does This Project Do?

You have real audio files (like `.mp3` bird recordings). You want the computer to listen to them and tell you:
> **"Did a Sparrow chirp in this recording? Yes or No?"**

```
 [ 🎵 Your MP3 Audio ]  --->  [ 🧠 Teachable Machine AI ]  --->  [ 🎉 "Sparrow Detected (100%)!" ]
```

---

## 📂 2. Super Simple Project Layout

You only have **3 main things** in this project:

```
BlueBird/
├── 📁 input_audio/                         <-- DROP YOUR REAL SOUND FILES HERE (.mp3 or .wav)
│   ├── house_sparrow_1.mp3
│   └── garden-sounds-with_birds.mp3
│
├── 📁 model/                               <-- YOUR GOOGLE TEACHABLE MACHINE FILES
│   ├── soundclassifier_with_metadata.tflite
│   └── labels.txt
│
└── 📄 classify.py                          <-- THE ONLY SCRIPT YOU NEED TO RUN!
```

---

## 🧠 3. How Does the Code Work?

1. **Loads the Sound**: Reads your `.mp3` or `.wav` file from the `input_audio/` folder.
2. **Scans Across Time**: Because nature recordings can be 1 or 2 minutes long, the script listens in **1-second windows** across the whole file.
3. **Asks the AI Model**: Feeds each 1-second sound snippet to your Google Teachable Machine model.
4. **Shows the Result**: Tells you if a sparrow was heard, what the highest confidence percentage was, and at what second the loudest chirp happened!

---

## 🚀 4. How to Run It

### To test ALL files in your folder:
```bash
python classify.py
```

### To test ONE specific audio file:
```bash
python classify.py input_audio/house_sparrow_1.mp3
```

---

## 📊 Example Output

```text
🎧 Listening to: house_sparrow_1.mp3
⏱️  Duration: 120.4 seconds
---------------------------------------------
🎉 SPARROW DETECTED! (Confidence: 100.0%)
📍 Loudest chirp heard around: 13.5 seconds
🔢 Total sparrow calls detected: 139
---------------------------------------------
```
