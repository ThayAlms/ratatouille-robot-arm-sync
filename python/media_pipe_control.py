import cv2
import serial
import time
import mediapipe as mp

# --- Conecta ao Arduino ---
arduino = serial.Serial('/dev/ttyUSB0', 9600)
time.sleep(2)

# --- Inicializa MediaPipe Pose ---
mp_drawing = mp.solutions.drawing_utils
mp_pose = mp.solutions.pose
pose = mp_pose.Pose(min_detection_confidence=0.5, min_tracking_confidence=0.5)

# --- Inicializa webcam ---
cap = cv2.VideoCapture(0)

# --- Função de suavização ---
def lerp(a, b, t=0.2):
    return a + (b - a) * t

# posições iniciais dos servos
left_servo_pos = 90
right_servo_pos = 90

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Converte para RGB
    image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    image.flags.writeable = False
    results = pose.process(image)
    image.flags.writeable = True
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

    if results.pose_landmarks:
        # desenha os pontos
        mp_drawing.draw_landmarks(image, results.pose_landmarks, mp_pose.POSE_CONNECTIONS)

        # --- Braço direito ---
        r_shoulder = results.pose_landmarks.landmark[mp_pose.PoseLandmark.RIGHT_SHOULDER]
        r_elbow = results.pose_landmarks.landmark[mp_pose.PoseLandmark.RIGHT_ELBOW]
        r_wrist = results.pose_landmarks.landmark[mp_pose.PoseLandmark.RIGHT_WRIST]

        # --- Braço esquerdo ---
        l_shoulder = results.pose_landmarks.landmark[mp_pose.PoseLandmark.LEFT_SHOULDER]
        l_elbow = results.pose_landmarks.landmark[mp_pose.PoseLandmark.LEFT_ELBOW]
        l_wrist = results.pose_landmarks.landmark[mp_pose.PoseLandmark.LEFT_WRIST]

        # --- Converte altura do braço em ângulo servo (simplificado) ---
        # MediaPipe normaliza Y de 0 (topo) a 1 (base)
        # Pulso acima do ombro = braço levantado
        right_target = int(180 * (1 - (r_wrist.y - r_shoulder.y)))  # escala 0-180
        left_target  = int(180 * (1 - (l_wrist.y - l_shoulder.y)))  # escala 0-180

        # Limita 0-180
        right_target = max(0, min(180, right_target))
        left_target = max(0, min(180, left_target))

        # --- Suaviza movimento ---
        right_servo_pos = lerp(right_servo_pos, right_target, 0.2)
        left_servo_pos = lerp(left_servo_pos, left_target, 0.2)

        # --- Envia para Arduino ---
        cmd = f"{int(left_servo_pos)},{int(right_servo_pos)}\n"
        arduino.write(cmd.encode('utf-8'))

        # --- Feedback visual ---
        cv2.putText(image, f"L:{int(left_servo_pos)} R:{int(right_servo_pos)}",
                    (10,30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0), 2)

    # --- Mostrar webcam ---
    cv2.imshow("RemyBot Real-Time Arm Control", image)

    if cv2.waitKey(1) & 0xFF == 27:  # ESC para sair
        break

cap.release()
cv2.destroyAllWindows()
