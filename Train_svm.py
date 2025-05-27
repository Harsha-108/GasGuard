import json
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.svm import LinearSVC
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import accuracy_score, confusion_matrix
import pandas as pd

# --- Configuration ---
JSON_FILE_PATH = 'feed.json'
# IMPORTANT: Labeling strategy - THIS IS A PLACEHOLDER!
# You MUST replace this with a proper labeling strategy for your data.
# For demonstration, we'll consider a high CO reading (field8) as a "non-false alarm".
CO_THRESHOLD_NON_FALSE_ALARM = 500  # Example: CO level in ppm

def load_and_preprocess_data(file_path):
    """Loads data from JSON, extracts features, and generates placeholder labels."""
    try:
        with open(file_path, 'r') as f:
            data = json.load(f)
    except FileNotFoundError:
        print(f"Error: The file {file_path} was not found.")
        return None, None
    except json.JSONDecodeError:
        print(f"Error: Could not decode JSON from {file_path}.")
        return None, None

    feeds = data.get('feeds', [])
    if not feeds:
        print("No 'feeds' data found in the JSON file.")
        return None, None

    features = []
    labels = []

    # Feature names corresponding to field1 through field8
    # From channel info: "field1":"LPG","field2":"Propane","field3":"Hydrogen","field4":"Smoke",
    # "field5":"Methane","field6":"Methane(MQ4)","field7":"CNG","field8":"CO"
    feature_keys = [f'field{i}' for i in range(1, 9)]

    for entry in feeds:
        try:
            current_features = [float(entry.get(key, 0.0)) for key in feature_keys]
            features.append(current_features)

            # --- Placeholder Labeling Logic ---
            co_value = float(entry.get('field8', 0.0))
            if co_value > CO_THRESHOLD_NON_FALSE_ALARM:
                labels.append(1)  # Non-false alarm
            else:
                labels.append(0)  # False alarm
            # --- End Placeholder Labeling Logic ---

        except (TypeError, ValueError) as e:
            print(f"Warning: Skipping entry due to data conversion error: {entry}. Error: {e}")
            continue

    if not features or not labels:
        print("No valid data could be extracted for training.")
        return None, None

    return np.array(features), np.array(labels)

def train_svm_model(X, y):
    """Trains a Linear SVM model."""
    if X is None or y is None or len(X) == 0:
        print("Cannot train model with no data.")
        return None, None, None, None, None # Added None for scaler

    # Stratify y if there are at least 2 samples of the minority class, and at least two classes.
    # If y has only one unique value, stratify=None.
    # If y has multiple unique values but one class has only 1 sample, stratify can cause errors.
    n_classes = len(np.unique(y))
    min_class_count = np.min(np.bincount(y)) if n_classes > 1 else 0
    
    stratify_param = y if n_classes >= 2 and min_class_count >= 2 else None

    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.25, random_state=42, stratify=stratify_param)

    if len(np.unique(y_train)) < 2 :
        print("Warning: Training data has only one class after split. SVM might not train effectively or produce trivial results.")
        print("This can happen with small datasets or highly imbalanced data combined with the placeholder labeling.")
        # We will return None and handle it.
        print("Cannot train LinearSVC with only one class in training data.")
        return None, None, None, None, None # Added None for scaler

    # Scale features
    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train)
    X_test_scaled = scaler.transform(X_test)

    # Determine dual parameter based on n_samples vs n_features
    # dual=True is preferred when n_samples > n_features.
    n_samples, n_features = X_train_scaled.shape
    dual_param = True if n_samples > n_features else False
    print(f"Using dual={dual_param} for LinearSVC (n_samples={n_samples}, n_features={n_features})")


    # Train LinearSVC model
    model = LinearSVC(random_state=42, tol=1e-4, max_iter=2000, dual=dual_param) # Changed dual="auto" to dual_param
    try:
        model.fit(X_train_scaled, y_train)
    except ValueError as e:
        print(f"Error during model training: {e}")
        print("This can happen if one class is missing in the training split or other data issues.")
        return None, None, None, None, None # Added None for scaler


    # Make predictions
    y_pred_train = model.predict(X_train_scaled)
    y_pred_test = model.predict(X_test_scaled)

    # Evaluate model
    train_accuracy = accuracy_score(y_train, y_pred_train)
    test_accuracy = accuracy_score(y_test, y_pred_test)
    
    print("\n--- Model Evaluation ---")
    print(f"Training Accuracy: {train_accuracy:.4f}")
    print(f"Test Accuracy: {test_accuracy:.4f}")
    
    print("\nConfusion Matrix (Test Set):")
    cm = confusion_matrix(y_test, y_pred_test)
    print(cm)
    
    # Displaying confusion matrix with labels for clarity
    # Assuming 0: False Alarm, 1: Non-False Alarm
    if len(np.unique(y_test)) < 2 and len(cm.ravel()) == 1 : # Only one class in test set
        if y_test[0] == 0: # All are actual negatives
            tn, fp, fn, tp = cm.ravel()[0], 0, 0, 0
        else: # All are actual positives
            tn, fp, fn, tp = 0, 0, 0, cm.ravel()[0]
    elif len(cm.ravel()) == 4: # Standard 2x2 matrix
         tn, fp, fn, tp = cm.ravel()
    else: # Fallback for unexpected cm shape
        print("Could not reliably unpack confusion matrix. Displaying raw ravelled form:")
        print(cm.ravel())
        tn, fp, fn, tp = 0,0,0,0 # Default to zeros to avoid further errors

    print(f"  True Negatives (Correctly False): {tn}")
    print(f"  False Positives (Incorrectly Non-False): {fp}")
    print(f"  False Negatives (Incorrectly False): {fn}")
    print(f"  True Positives (Correctly Non-False): {tp}")


    return model, scaler, X_test_scaled, y_test, y_pred_test

def main():
    """Main function to run the SVM training pipeline."""
    X, y = load_and_preprocess_data(JSON_FILE_PATH)

    if X is None or y is None:
        print("Exiting due to data loading issues.")
        return

    if len(X) == 0 or len(y) == 0:
        print("No data available for training. Exiting.")
        return
        
    if len(np.unique(y)) < 2:
        print("\nWARNING: The entire dataset contains only one class based on the current labeling strategy (CO_THRESHOLD_NON_FALSE_ALARM).")
        print("SVM training requires at least two classes for meaningful classification.")
        print("Please adjust your CO_THRESHOLD_NON_FALSE_ALARM or provide data that results in multiple classes.")
        print("The script will attempt to proceed, but the model will likely not be useful.")
        # Even if only one class, we can still show "coefficients" if we force a dummy model,
        # but it's better to halt and inform the user.

    model, scaler, _, _, _ = train_svm_model(X, y)

    if model and scaler: # Check if scaler is also successfully created
        print("\n--- SVM Model Parameters (for ESP32) ---")
        # The decision function is: w^T * x + b
        # For LinearSVC, model.coef_ are the weights (w)
        # model.intercept_ is the bias (b)
        # The features (x) need to be scaled using the same scaler.mean_ and scaler.scale_
        # before applying the weights.
        # Decision = sum( (X_scaled[i] * coef_[i]) ) + intercept_
        # X_scaled[i] = (X_raw[i] - scaler.mean_[i]) / scaler.scale_[i]

        print("Scaler Means (for each feature):")
        print(scaler.mean_)
        print("\nScaler Scales (standard deviations for each feature):")
        print(scaler.scale_)
        print("\nSVM Coefficients (weights for each scaled feature):")
        print(model.coef_[0]) # Assuming binary classification, coef_ will be (1, n_features)
        print("\nSVM Intercept:")
        print(model.intercept_[0]) # Assuming binary classification

        print("\n--- How to use on ESP32 (Conceptual) ---")
        print("1. Store the 8 'Scaler Means', 8 'Scaler Scales', 8 'SVM Coefficients', and 1 'SVM Intercept' as constants in your ESP32 code.")
        print("2. For new sensor readings (raw_f1, ..., raw_f8):")
        print("   a. Scale each feature: scaled_fi = (raw_fi - mean_i) / scale_i")
        print("   b. Calculate decision value: sum(scaled_fi * coef_i for i in 1..8) + intercept")
        print("3. If decision value >= 0, classify as 'Non-False Alarm' (or class 1). Otherwise, 'False Alarm' (or class 0).")
    else:
        print("\nSVM Model training failed or was skipped due to data issues (e.g., only one class after split, or other training errors).")
        print("Please check your data, labeling strategy (CO_THRESHOLD_NON_FALSE_ALARM), and ensure you have enough diverse samples for both classes.")

if __name__ == "__main__":
    main()
