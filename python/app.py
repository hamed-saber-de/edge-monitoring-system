from flask import Flask, render_template
import serial
import threading
import time

app = Flask(__name__)

# -----------------------------
# SERIAL CONNECTION
# -----------------------------

arduino = serial.Serial('/dev/ttyACM0', 9600)

# -----------------------------
# GLOBAL VARIABLES
# -----------------------------

temperature = 0
humidity = 0

status_color = "green"
warning_color = "gray"

status_text = "NORMAL"
warning_text = "OFF"

# -----------------------------
# SERIAL READER THREAD
# -----------------------------

def read_serial():

    global temperature
    global humidity

    global status_color
    global warning_color

    global status_text
    global warning_text

    while True:

        try:

            data = arduino.readline().decode().strip()

            print(data)

            # Ignore startup messages
            if not data.startswith("TEMP:"):

                continue

            # -----------------------------
            # PARSE SERIAL DATA
            # -----------------------------

            parts = data.split(",")

            temperature = float(parts[0].split(":")[1])
            humidity = float(parts[1].split(":")[1])

            # -----------------------------
            # STATUS LOGIC
            # -----------------------------

            if temperature < 25.3:

                status_text = "NORMAL"
                warning_text = "OFF"

                status_color = "green"
                warning_color = "gray"

            elif temperature < 28:

                status_text = "WARNING"
                warning_text = "OFF"

                status_color = "orange"
                warning_color = "gray"

            elif temperature < 30:

                status_text = "CRITICAL"
                warning_text = "ACTIVE"

                status_color = "red"

                if int(time.time()) % 10 == 0:

                    warning_color = "red"

                else:

                    warning_color = "gray"

            else:

                status_text = "HIGH CRITICAL"
                warning_text = "HIGH ALERT"

                status_color = "red"

                if int(time.time() * 1000) % 800 < 400:

                    warning_color = "red"

                else:

                    warning_color = "gray"

        except Exception as e:

            print("Serial Error:", e)

        time.sleep(0.05)

# -----------------------------
# START THREAD
# -----------------------------

thread = threading.Thread(
    target=read_serial,
    daemon=True
)

thread.start()

# -----------------------------
# WEB PAGE
# -----------------------------

@app.route("/")

def home():

    return render_template(

        "index.html",

        temperature=temperature,
        humidity=humidity,

        status_color=status_color,
        warning_color=warning_color,

        status_text=status_text,
        warning_text=warning_text
    )

# -----------------------------
# MAIN
# -----------------------------

if __name__ == "__main__":

    app.run(
        host="0.0.0.0",
        port=5000
    )
