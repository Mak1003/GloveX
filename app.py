import streamlit as st
import serial
import serial.tools.list_ports
import threading
import queue
import time
import pyttsx3

# ============================================================
# Streamlit Setup
# ============================================================
st.set_page_config(page_title="GloveX+ Serial Monitor", layout="wide")
st.title("🧤 GloveX+ Web Serial Monitor + Gesture Translator")
st.markdown("Real-time ESP32 serial monitor with gesture speech output")

# ============================================================
# Initialize App State
# ============================================================
if "serial_queue" not in st.session_state:
    st.session_state.serial_queue = queue.Queue()
if "serial_thread" not in st.session_state:
    st.session_state.serial_thread = None
if "stop_serial" not in st.session_state:
    st.session_state.stop_serial = threading.Event()
if "connected" not in st.session_state:
    st.session_state.connected = False
if "current_port" not in st.session_state:
    st.session_state.current_port = None
if "last_gesture" not in st.session_state:
    st.session_state.last_gesture = ""
if "history" not in st.session_state:
    st.session_state.history = []

# ============================================================
# Functions
# ============================================================
def list_ports():
    """List all COM ports"""
    ports = serial.tools.list_ports.comports()
    return [p.device for p in ports]


def serial_reader(port, baudrate, q, stop_event):
    """Continuously read from serial port"""
    try:
        ser = serial.Serial(port, baudrate, timeout=1)
        q.put(f"✅ Connected to {port} at {baudrate} baud.")
        while not stop_event.is_set():
            if ser.in_waiting > 0:
                line = ser.readline().decode(errors="ignore").strip()
                if line:
                    q.put(line)
        ser.close()
    except Exception as e:
        q.put(f"❌ Serial Error: {e}")


def speak_text(text):
    """Speak detected gestures"""
    try:
        engine = pyttsx3.init()
        engine.setProperty("rate", 170)
        engine.setProperty("volume", 1.0)
        voices = engine.getProperty("voices")
        if voices:
            engine.setProperty("voice", voices[0].id)
        engine.say(text)
        engine.runAndWait()
        engine.stop()
    except Exception as e:
        st.warning(f"TTS error: {e}")


def auto_connect():
    """Auto-detect and connect to ESP32"""
    ports = list_ports()
    for port in ports:
        try:
            ser = serial.Serial(port, 115200, timeout=1)
            ser.close()
            return port
        except Exception:
            continue
    return None

# ============================================================
# Auto Connect + Serial Start
# ============================================================
if not st.session_state.connected:
    port = auto_connect()
    if port:
        st.session_state.stop_serial.clear()
        thread = threading.Thread(
            target=serial_reader,
            args=(port, 115200, st.session_state.serial_queue, st.session_state.stop_serial),
            daemon=True,
        )
        thread.start()
        st.session_state.serial_thread = thread
        st.session_state.connected = True
        st.session_state.current_port = port
        st.success(f"🔗 Auto-connected to ESP32 on {port}")
    else:
        st.error("⚠️ No ESP32 detected. Plug it in and refresh.")
        st.stop()

# ============================================================
# Layout
# ============================================================
col1, col2 = st.columns(2)

# Left Panel — Live Serial Monitor
with col1:
    st.subheader("📡 Live Serial Monitor (Real-Time Feed)")
    serial_feed = st.empty()

# Right Panel — Gesture + History
with col2:
    st.subheader("🖐 Detected Gesture")
    gesture_box = st.empty()
    st.markdown("---")
    st.subheader("🕒 Gesture History")
    history_box = st.empty()

# ============================================================
# Main Loop — Read Serial Queue
# ============================================================
serial_lines = []

while True:
    while not st.session_state.serial_queue.empty():
        line = st.session_state.serial_queue.get()

        # Store raw serial lines for display
        serial_lines.insert(0, line)
        if len(serial_lines) > 20:
            serial_lines = serial_lines[:20]

        # Display the live serial feed
        serial_feed.text("\n".join(serial_lines))

        # Gesture detection
        if "Gesture" in line or "🖐" in line:
            gesture = line.split(":", 1)[-1].strip()
            if gesture and gesture != st.session_state.last_gesture:
                st.session_state.last_gesture = gesture
                st.session_state.history.insert(0, f"{time.strftime('%H:%M:%S')} - {gesture}")

                # Display the gesture on screen
                gesture_box.markdown(
                    f"""
                    <div style='text-align:center; padding:15px; background-color:#111; border-radius:12px;'>
                        <h2 style='color:#00FFFF;'>Detected:</h2>
                        <h1 style='color:#FFD700;'>{gesture}</h1>
                    </div>
                    """,
                    unsafe_allow_html=True,
                )

                # Speak the gesture instantly
                speak_text(gesture)

                # Update history
                history_box.text("\n".join(st.session_state.history[:10]))

    time.sleep(0.1)
