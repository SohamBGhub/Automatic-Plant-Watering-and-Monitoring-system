"""
Plant Disease Detection using TensorFlow Lite

Loads a trained .tflite plant-disease classification model and runs
inference on a single stored test image (38-class PlantVillage-style
label set). This is a one-shot script: it does not use a live camera
feed and does not loop or watch a directory.

Usage:
    python3 disease_detection.py

Before running, update MODEL_PATH and IMAGE_PATH below to match your
own file locations.
"""

import tflite_runtime.interpreter as tflite
import cv2
import numpy as np

# ---------------------------------------------------------------------------
# Configuration - update these paths for your setup
# ---------------------------------------------------------------------------
MODEL_PATH = "/home/pi/Arduino/plant_disease_model.tflite"
IMAGE_PATH = "/home/pi/Arduino/test_img/test/YourTestImage.jpg"

# ---------------------------------------------------------------------------
# Disease class labels (38 classes, PlantVillage-style)
# ---------------------------------------------------------------------------
diseases = {
    "0": "Apple___Apple_scab",
    "1": "Apple___Black_rot",
    "2": "Apple___Cedar_apple_rust",
    "3": "Apple___healthy",
    "4": "Blueberry___healthy",
    "5": "Cherry_(including_sour)___Powdery_mildew",
    "6": "Cherry_(including_sour)___healthy",
    "7": "Corn_(maize)___Cercospora_leaf_spot Gray_leaf_spot",
    "8": "Corn_(maize)___Common_rust_",
    "9": "Corn_(maize)___Northern_Leaf_Blight",
    "10": "Corn_(maize)___healthy",
    "11": "Grape___Black_rot",
    "12": "Grape___Esca_(Black_Measles)",
    "13": "Grape___Leaf_blight_(Isariopsis_Leaf_Spot)",
    "14": "Grape___healthy",
    "15": "Orange___Haunglongbing_(Citrus_greening)",
    "16": "Peach___Bacterial_spot",
    "17": "Peach___healthy",
    "18": "Pepper_bell___Bacterial_spot",
    "19": "Pepper_bell___healthy",
    "20": "Potato___Early_blight",
    "21": "Potato___Late_blight",
    "22": "Potato___healthy",
    "23": "Raspberry___healthy",
    "24": "Soybean___healthy",
    "25": "Squash___Powdery_mildew",
    "26": "Strawberry___Leaf_scorch",
    "27": "Strawberry___healthy",
    "28": "Tomato___Bacterial_spot",
    "29": "Tomato___Early_blight",
    "30": "Tomato___Late_blight",
    "31": "Tomato___Leaf_Mold",
    "32": "Tomato___Septoria_leaf_spot",
    "33": "Tomato___Spider_mites Two-spotted_spider_mite",
    "34": "Tomato___Target_Spot",
    "35": "Tomato___Tomato_Yellow_Leaf_Curl_Virus",
    "36": "Tomato___Tomato_mosaic_virus",
    "37": "Tomato___healthy",
}


def main():
    # Load the TFLite model
    interpreter = tflite.Interpreter(model_path=MODEL_PATH)
    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    # Load and preprocess the image
    image = cv2.imread(IMAGE_PATH)
    if image is None:
        raise FileNotFoundError(f"Could not read image at {IMAGE_PATH}")

    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)  # Convert to RGB
    image = cv2.resize(image, (224, 224))           # Resize to model input
    image = image / 255.0                            # Normalize to [0, 1]
    image = np.expand_dims(image, axis=0).astype(np.float32)

    # Run inference
    interpreter.set_tensor(input_details[0]["index"], image)
    interpreter.invoke()
    output_data = interpreter.get_tensor(output_details[0]["index"])
    predicted_class = int(np.argmax(output_data))
    predicted_label = diseases[str(predicted_class)]

    # Display result
    if "healthy" in predicted_label.lower():
        print("The plant is healthy")
    else:
        print(f"The plant is unhealthy. Detected disease: {predicted_label}")


if __name__ == "__main__":
    main()
