# 🐦 BlueBird - Sparrow Sound Audio Classifier

A simple project that listens to real audio recordings (`.mp3` or `.wav`) and uses an AI model from **Google Teachable Machine** to detect **sparrow bird sounds**.

---

## 📁 Super Simple Project Structure

```
BlueBird/
├── input_audio/                         # 📁 Drop your .mp3 or .wav files here!
│   ├── house_sparrow_1.mp3
│   └── garden-sounds-with_birds.mp3
├── model/                               # 🧠 Google Teachable Machine model
│   ├── soundclassifier_with_metadata.tflite
│   └── labels.txt
├── classify.py                          # 🚀 The ONE Python script to run!
├── requirements.txt                     # 📦 Needed Python packages
└── docs/
    └── PROJECT_EXPLAINED.md             # 📖 Simple explanation for beginners
```

---

## 🚀 How to Run in 2 Steps

### Step 1: Install Requirements
```bash
pip install -r requirements.txt
```

### Step 2: Classify Your Audio Files
To classify **all** audio files inside `input_audio/`:
```bash
python classify.py
```

Or classify a **specific** file:
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
