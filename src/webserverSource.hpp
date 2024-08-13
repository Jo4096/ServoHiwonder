#ifndef webserverSource
#define webserverSource
#include <Arduino.h>

const char PAGE_MAIN[] PROGMEM = R"====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Servo Controller</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            color: #333;
            text-align: center;
            padding: 20px;
            margin: 0;
        }
        h1 {
            color: #444;
            font-size: 2em;
            margin-bottom: 20px;
        }
        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            max-width: 600px;
            margin: auto;
            background: #fff;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        .display {
            margin: 15px 0;
            padding: 15px;
            border: 2px solid #ddd;
            border-radius: 5px;
            width: 80%;
            background: #fafafa;
            font-size: 1.1em;
        }

        button {
            margin: 10px 5px;
            padding: 10px 20px;
            font-size: 1em;
            border: none;
            border-radius: 5px;
            background-color: #007BFF;
            color: white;
            cursor: pointer;
            transition: background-color 0.3s ease;
        }
        button:hover {
            background-color: #0056b3;
        }
        form {
            margin: 20px 0;
        }
        label {
            padding: 10px 20px;
            border: 1px solid #ddd;
            border-radius: 5px;
            background: #f8f8f8;
            cursor: pointer;
            transition: background-color 0.3s ease;
        }
        label:hover {
            background-color: #e8e8e8;
        }
    </style>
</head>
<body>
    <h1>Servo Controller</h1>
    <div class="container">
        <div class="display" id="CurrentServoChosen">Current Servo ID: N/A</div>
        <div class="display" id="CurrentServoPosition">Current Position: N/A</div>
        <button onclick="BUTTON_MOVE_CURRENT_SERVO_LEFT()">-10% Pos</button>
        <button onclick="BUTTON_MOVE_CURRENT_SERVO_RIGHT()">+10% Pos</button>
        <button onclick="buttonPressLeft()">before</button>
        <button onclick="buttonPressRight()">next</button>
        <button onclick="tutorial()">How to use</button>
    </div>

    <script type="text/javascript">
        function createXmlHttpObject() {
            if (window.XMLHttpRequest) {
                return new XMLHttpRequest();
            } else {
                return new ActiveXObject("Microsoft.XMLHTTP");
            }
        }

        var xmlHttp = createXmlHttpObject();

        function BUTTON_MOVE_CURRENT_SERVO_LEFT() {
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "BUTTON_MOVE_CURRENT_SERVO_LEFT", false);
            xhttp.send();
        }

        function BUTTON_MOVE_CURRENT_SERVO_RIGHT() {
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "BUTTON_MOVE_CURRENT_SERVO_RIGHT", false);
            xhttp.send();
        }

        function buttonPressLeft() {
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "BUTTON_LEFT", false);
            xhttp.send();
        }

        function buttonPressRight() {
            var xhttp = new XMLHttpRequest();
            xhttp.open("PUT", "BUTTON_RIGHT", false);
            xhttp.send();
        }

        function tutorial() {
            window.location.href = "https://youtu.be/dQw4w9WgXcQ?si=Yx6ZmPS1LU-ojNuy";
        }

        function updateDisplays() {
            xmlHttp.open("GET", "/xml", true);
            xmlHttp.onreadystatechange = function () {
                if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
                    var xmlResponse = xmlHttp.responseXML;
                    var CurrentServoChosen = xmlResponse.getElementsByTagName("CurrentServoChosen")[0].textContent;
                    var CurrentServoPosition = xmlResponse.getElementsByTagName("CurrentServoPosition")[0].textContent;
                    var CurrentBattery = xmlResponse.getElementsByTagName("CurrentBattery")[0].textContent;

                    document.getElementById("CurrentServoChosen").textContent = "Current Servo ID: " + CurrentServoChosen;
                    document.getElementById("CurrentServoPosition").textContent = "Current Position: " + CurrentServoPosition;
                }
            };
            xmlHttp.send(null);
        }

        setInterval(updateDisplays, 1000);
    </script>
</body>
</html>
)====";
#endif
