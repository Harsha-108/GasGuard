# GasGuard: An IoT-Based Gas Leakage Detection and Alert System with SVM Enhancement

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub issues](https://img.shields.io/github/issues/Harsha-108/GasGuard)](https://github.com/Harsha-108/GasGuard/issues)
[![GitHub forks](https://img.shields.io/github/forks/Harsha-108/GasGuard)](https://github.com/Harsha-108/GasGuard/network)
[![GitHub stars](https://img.shields.io/github/stars/Harsha-108/GasGuard)](https://github.com/Harsha-108/GasGuard/stargazers)

## 📜 Abstract

**GasGuard** is an innovative Internet of Things (IoT) solution meticulously designed to provide **instant alerts for gas leakages**, ensuring safety in various environments. This system integrates a robust hardware framework comprising **MQ-2, MQ-4, and MQ-9 gas sensors**, all managed by an **ESP32 microcontroller**, to continuously monitor ambient gas concentrations.

When gas levels exceed predefined safety thresholds, GasGuard activates a multi-tiered alert system:
* **Local Alerts:** Immediate visual warnings via **LEDs** and audible alarms through a **buzzer**.
* **Remote Notifications:** Critical alerts sent as **SMS messages via a GSM module** to designated contacts, ensuring swift response even when users are off-site.

To enhance data analysis and system intelligence, GasGuard utilizes:
* **ThingSpeak:** A cloud-based platform for real-time data logging, storage, and visualization of gas concentration levels.
* **Support Vector Machine (SVM):** A machine learning model implemented to significantly **reduce false alarms** and improve the overall **accuracy of gas detection**.

GasGuard is engineered for **high energy efficiency** and **scalability**, making it a versatile solution suitable for a wide range of applications, from residential homes to large-scale industrial facilities, delivering reliable and intelligent safety protection.

This project is based on the research paper: *Dubey, K. B., Chauhan, A., Shrivastava, A., & Chaudhary, H. "GasGuard: An IoT-Based Gas Leakage Detection and Alert System." ABES Institute of Technology, Ghaziabad, India.*

---

## ✨ Key Features

* **Multi-Gas Detection Capability:** Employs a suite of sensors (MQ-2, MQ-4, MQ-9) to detect a variety of hazardous gases, including methane, propane, and carbon monoxide.
* **Powerful ESP32 Core:** Utilizes the ESP32 microcontroller for efficient data acquisition, processing, sensor integration, and wireless communication tasks.
* **Comprehensive Alert System:**
    * **Immediate Local Alarms:** High-visibility LEDs and a loud buzzer for on-site warnings.
    * **Reliable Remote Alerts:** GSM module ensures SMS notifications reach emergency contacts promptly, regardless of their location.
* **Cloud-Powered Data Analytics:** Seamless integration with ThingSpeak for:
    * Real-time data logging and monitoring.
    * Historical data storage for trend analysis and incident review.
    * Accessible visualization of gas concentration patterns.
* **AI-Enhanced Accuracy (SVM):**
    * Implements a Support Vector Machine model to intelligently classify gas readings.
    * Significantly minimizes false positives by learning from historical sensor data.
    * The SVM model is optimized to run directly on the ESP32 for real-time inference and decision-making.
* **Energy-Efficient and Scalable Design:**
    * Optimized for low power consumption, extending operational life in battery-powered deployments.
    * Modular design allows for scalability to cover diverse environments and requirements.

---

## 📂 Repository Structure

The project is organized as follows:

```text
.
├── GasGuard_SVM.ino    # Arduino sketch for the ESP32 microcontroller.
│                       # Handles sensor data acquisition, SVM model inference,
│                       # alert activation (LED, buzzer, SMS), and data
│                       # transmission to ThingSpeak.
├── LICENSE             # Contains the MIT License details for the project.
├── README.md           # This comprehensive README file.
├── Train_svm.py        # Python script used for training the Support Vector
│                       # Machine (SVM) model. It processes sensor data
│                       # (likely from `feed.json` or similar collected data)
│                       # to generate the model parameters.
└── feed.json           # A sample JSON file. This could represent:
                        #   - Example training data for the SVM model.
                        #   - Configuration parameters for the system.
                        #   - Sample sensor readings for testing or demonstration.
#   - Example training data for the SVM model.
#   - Configuration parameters for the system.
#   - Sample sensor readings for testing or demonstration.


---

## 🛠️ Technology Stack

GasGuard leverages a combination of hardware and software technologies:

* **Hardware:**
    * Microcontroller: **ESP32** (known for its dual-core processing, Wi-Fi, and Bluetooth capabilities)
    * Gas Sensors:
        * **MQ-2:** Sensitive to Methane, Butane, LPG, Smoke.
        * **MQ-4:** Sensitive to Methane (CH4), Natural Gas.
        * **MQ-9:** Sensitive to Carbon Monoxide (CO), Flammable Gases.
    * Alerting Components: **LEDs** (visual), **Buzzer** (audible).
    * Communication Module: **GSM Module** (e.g., SIM800L or SIM900A, as depicted in the research paper's system architecture) for SMS alerts.
* **Software & Cloud Platform:**
    * Embedded Programming: **Arduino C/C++** (for the ESP32 sketch `GasGuard_SVM.ino`).
    * Machine Learning:
        * Language: **Python 3.x** (for the SVM training script `Train_svm.py`).
        * Libraries: **Scikit-learn** (for SVM implementation), **Pandas** (for data manipulation), **NumPy** (for numerical operations).
    * Cloud IoT Platform: **ThingSpeak** (for data aggregation, visualization, and analytics).
    * Machine Learning Model: **Support Vector Machine (SVM)** (for classification and false alarm reduction).

---

## 🚀 Getting Started

To set up and run the GasGuard system, follow these steps:

### Prerequisites

1.  **Hardware Components:**
    * An ESP32 development board.
    * MQ-2, MQ-4, and MQ-9 gas sensors.
    * A compatible GSM Module (e.g., SIM900A or SIM800L) with an activated SIM card (ensure it supports 2G if using older modules).
    * Standard LEDs (e.g., red for alert, green for normal).
    * A 5V active buzzer.
    * Breadboard, connecting wires (jumper wires), and appropriate resistors (refer to sensor datasheets and the system architecture in the research paper, Fig. 1).
    * A stable power supply for the ESP32 and GSM module (typically 5V for ESP32, check GSM module voltage requirements).
2.  **Software & Accounts:**
    * **Arduino IDE** (latest version recommended) or **PlatformIO** (for more advanced users).
    * **ESP32 Board Support Package:** Install via Arduino IDE's Board Manager or PlatformIO's configuration.
    * **Python 3.x** installed on your computer for model training.
    * **Required Python Libraries:** Install using pip:
        ```bash
        pip install scikit-learn pandas numpy
        # Install any other libraries explicitly imported in Train_svm.py
        ```
    * A **ThingSpeak Account:**
        * Sign up at [ThingSpeak.com](https://thingspeak.com/).
        * Create a new channel with fields corresponding to the data you want to log (e.g., MQ-2 reading, MQ-4 reading, MQ-9 reading, SVM status). Note down the **Channel ID** and **Write API Key**.
    * **Arduino Libraries:**
        * Check the `#include` statements in `GasGuard_SVM.ino`. You will likely need libraries for:
            * ESP32 Wi-Fi communication.
            * GSM module communication (e.g., `SoftwareSerial` might be used, or a specific library for your GSM module).
            * Any specific libraries for the MQ sensors if they require more than analog reads.
        * Install these libraries via the Arduino IDE's Library Manager.

### Hardware Assembly

Carefully assemble the hardware components based on the system architecture diagram provided in the research paper (Figure 1). Key connections include:
* **MQ Sensors:** Connect the analog output pins of the MQ sensors to available analog input pins on the ESP32. Ensure correct VCC and GND connections.
* **LEDs & Buzzer:** Connect LEDs (with current-limiting resistors) and the buzzer to digital output pins of the ESP32.
* **GSM Module:**
    * Connect the TXD pin of the GSM module to an RXD-capable pin on the ESP32 (often using `SoftwareSerial` or a hardware serial port).
    * Connect the RXD pin of the GSM module to a TXD-capable pin on the ESP32.
    * Provide appropriate power and ground to the GSM module. **Note:** GSM modules can be power-hungry, especially during network registration and transmission; ensure your power supply can handle the current spikes.
* **Power:** Ensure all components are powered correctly and share a common ground.

### 1. SVM Model Training (`Train_svm.py`)

The SVM model is critical for enhancing the system's accuracy and minimizing false alarms.

1.  **Data Collection & Preparation:**
    * Collect comprehensive sensor data from your MQ-2, MQ-4, and MQ-9 sensors under diverse, known conditions:
        * Normal ambient air.
        * Presence of target gases (methane, propane, carbon monoxide) at various concentrations (low, medium, high).
        * Simulate conditions that might cause false positives with traditional threshold-based systems.
    * Structure this data into a suitable format for training (e.g., CSV). Each data point should include sensor readings as features and a label indicating the condition (e.g., 0 for normal, 1 for methane leak, 2 for CO leak, etc.).
    * The `feed.json` file in the repository might serve as an example of data structure or could be a placeholder for your collected training data.
2.  **Model Training Execution:**
    * Modify `Train_svm.py` to:
        * Load your prepared dataset.
        * Preprocess the data as needed (e.g., scaling, normalization).
        * Configure SVM parameters (kernel type, C, gamma – experimentation might be needed).
    * Run the training script from your terminal:
        ```bash
        python Train_svm.py
        ```
3.  **Model Output & Integration:**
    * The `Train_svm.py` script should output the parameters of the trained SVM model. This typically includes:
        * Support Vectors.
        * Coefficients (alphas or dual coefficients).
        * Intercept (rho or b).
        * Kernel parameters if applicable.
    * These parameters need to be **manually transferred or programmatically converted** into a format suitable for embedding within the `GasGuard_SVM.ino` Arduino sketch. This often involves creating C/C++ arrays or structures to store these values for the SVM prediction function on the ESP32. The research paper mentions the model size is around 135 KB, implying a relatively compact model suitable for microcontrollers.

### 2. ESP32 Setup & Programming (`GasGuard_SVM.ino`)

1.  **Configure the Arduino Sketch:**
    * Open `GasGuard_SVM.ino` in your Arduino IDE or PlatformIO.
    * **Pin Definitions:** Update the pin numbers for sensors (MQ-2, MQ-4, MQ-9), LEDs, buzzer, and GSM module (TX, RX pins) to match your actual hardware connections on the ESP32.
    * **Network Credentials:**
        * If using Wi-Fi for ThingSpeak: Enter your Wi-Fi SSID and password.
    * **ThingSpeak Configuration:** Input your ThingSpeak **Channel ID** and **Write API Key**.
    * **GSM Configuration:** Enter the phone number(s) to which SMS alerts should be sent.
    * **SVM Model Integration:**
        * Carefully embed the SVM model parameters (support vectors, coefficients, intercept) obtained from the Python training script into the designated arrays or variables within the sketch.
        * Ensure the SVM prediction logic in the sketch correctly uses these parameters.
    * **Thresholds (Optional but Recommended):** While SVM enhances detection, you might still want basic thresholds for immediate critical alerts or as a fallback.
2.  **Compile and Upload:**
    * In the Arduino IDE/PlatformIO, select the correct ESP32 board type (e.g., "ESP32 Dev Module").
    * Select the correct COM port to which your ESP32 is connected.
    * Compile the sketch to check for errors.
    * Upload the compiled sketch to your ESP32.
    * Open the Serial Monitor (set baud rate, typically 115200) to observe debug messages, sensor readings, and system status.

---

## ⚙️ How It Works: System Workflow

The GasGuard system operates through a well-defined workflow:

1.  **Initialization:**
    * Upon power-up, the ESP32 initializes its peripherals, sensors, GSM module, and Wi-Fi connection (if used for ThingSpeak).
    * It attempts to connect to the GSM network.
2.  **Continuous Sensing:**
    * The ESP32 periodically reads analog values from the MQ-2, MQ-4, and MQ-9 gas sensors.
    * These raw readings might undergo initial processing like averaging or calibration.
3.  **Data Processing & SVM Inference:**
    * The collected sensor readings (features) are fed into the embedded SVM model.
    * The SVM model classifies the current atmospheric condition based on its training (e.g., "Normal," "Methane Detected," "CO Detected," "High Risk").
4.  **Alert Generation & Notification:**
    * **Decision Logic:** Based on the SVM output (and potentially basic thresholds as a backup):
        * If a hazardous condition is identified:
            * **Local Alarms:** The corresponding LED(s) are activated (e.g., red LED blinks), and the buzzer sounds.
            * **Remote SMS Alert:** The ESP32 commands the GSM module to send an SMS message containing details of the alert (e.g., type of gas, location if configured) to the pre-programmed emergency phone number(s).
        * If conditions are normal, the system remains in monitoring mode (e.g., a green LED might indicate normal status).
5.  **Cloud Data Logging (ThingSpeak):**
    * Periodically, or upon significant events, the ESP32 sends the latest sensor readings and SVM status to the configured ThingSpeak channel using its Write API Key.
    * This allows for:
        * Remote real-time monitoring of gas levels via the ThingSpeak dashboard.
        * Storage of historical data for trend analysis, pattern recognition, and post-incident investigation.
        * Creation of charts and visualizations.
6.  **Fault Detection (Mentioned in Paper):**
    * The machine learning model can also help identify sensor failures or anomalous readings that deviate significantly from normal patterns, contributing to system reliability.

---

## 📊 Performance Highlights (Derived from the Research Paper)

The integration of the SVM model significantly enhances the GasGuard system's performance, particularly in terms of accuracy and reliability:

| Metric                | Formula                                                    | Result (%) | Notes                                                                 |
| :-------------------- | :--------------------------------------------------------- | :--------- | :-------------------------------------------------------------------- |
| **Accuracy** | `(TP + TN) / (TP + TN + FP + FN) * 100`                    | **95.2** | Overall correctness in identifying normal and hazardous conditions.   |
| **Precision** | `TP / (TP + FP) * 100`                                     | **94.8** | Proportion of actual hazardous events among all reported alerts.      |
| **Recall (Sensitivity)** | `TP / (TP + FN) * 100`                                     | **96.3** | Ability to identify all genuine hazardous gas leaks.                  |
| **F1 Score** | `2 * (Precision * Recall) / (Precision + Recall) * 100`    | **95.5** | Harmonic mean of Precision and Recall, balancing both metrics.        |
| **False Alarm Rate (FAR)** | `FP / (FP + TN) * 100`                                     | **3.1** | Proportion of normal cases mistakenly identified as hazardous. (Lower is better) |
| **Execution Time (ms)** | -                                                          | **~4.7** | Time taken by the SVM model on ESP32 for a single classification.     |
| **Model Size (KB)** | -                                                          | **~135** | Size of the deployed SVM model, suitable for microcontroller memory. |

**Detection Accuracy for Specific Gases:**

| Gas Type         | Detection Accuracy (%) | False Alarm Rate (%) | Response Time (s) |
| :--------------- | :--------------------- | :------------------- | :---------------- |
| Methane          | 98.5                   | 1.2                  | 5.3               |
| Carbon Monoxide  | 97.2                   | 0.9                  | 4.8               |
| Propane          | 96.8                   | 1.0                  | 5.1               |

These results underscore the system's capability to accurately detect various gases while maintaining a low false alarm rate, making it a reliable safety solution.

---

## 💡 Potential Enhancements & Future Work

While GasGuard is a robust system, further improvements could include:
* **Power Optimization:** Advanced sleep modes for the ESP32 and GSM module for battery-powered deployments.
* **Scalability:** Developing a mesh network of GasGuard units for larger area coverage.
* **User Interface:** A mobile application or web dashboard for more interactive control and data visualization beyond ThingSpeak.
* **Sensor Calibration:** Implementing an automated or semi-automated sensor calibration routine.
* **Enhanced Security:** Implementing encryption for data transmission to ThingSpeak and secure OTA (Over-The-Air) updates.
* **Wider Gas Detection:** Integrating sensors for other relevant hazardous gases based on specific application needs.
* **Adaptive Learning:** Exploring online learning techniques for the SVM model to adapt to changing environmental conditions over time.

---

## 🤝 Contributing

Contributions are highly encouraged to make GasGuard even better! If you have ideas for improvements, bug fixes, or new features, please follow these steps:

1.  **Fork** the repository ([https://github.com/Harsha-108/GasGuard](https://github.com/Harsha-108/GasGuard)).
2.  Create a new **branch** for your feature or fix (`git checkout -b feature/your-amazing-feature` or `bugfix/issue-description`).
3.  Make your **changes** and **commit** them with clear, descriptive messages (`git commit -m 'Add some amazing feature'`).
4.  **Push** your changes to your forked repository (`git push origin feature/your-amazing-feature`).
5.  Open a **Pull Request** against the `main` branch of the original `Harsha-108/GasGuard` repository.
6.  Clearly describe your changes and the problem they solve in the Pull Request.

Please ensure your code adheres to good coding practices and includes comments where necessary.

---

## 📄 License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file in the repository for full details.
