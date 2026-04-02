import cv2
import datetime

# Replace with your ESP32 IP
stream_url = "http://192.168.x.x:81/stream"

cap = cv2.VideoCapture(stream_url)

# HOG human detector
hog = cv2.HOGDescriptor()
hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())

while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab frame")
        break

    # Resize for faster processing
    frame = cv2.resize(frame, (640, 480))

    # Detect humans
    boxes, weights = hog.detectMultiScale(frame, winStride=(8,8))

    human_detected = False

    for (x, y, w, h) in boxes:
        human_detected = True
        cv2.rectangle(frame, (x,y), (x+w,y+h), (0,255,0), 2)

    # Save image if human detected
    if human_detected:
        filename = "human_" + datetime.datetime.now().strftime("%Y%m%d_%H%M%S") + ".jpg"
        cv2.imwrite(filename, frame)
        print("Saved:", filename)

    cv2.imshow("ESP32 Stream", frame)

    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()