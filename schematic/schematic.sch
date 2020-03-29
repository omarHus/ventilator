EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Ventilator"
Date ""
Rev "1A"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_Module:Arduino_UNO_R2 A1
U 1 1 5E80F89B
P 5900 3050
F 0 "A1" H 5900 4231 50  0000 C CNN
F 1 "Arduino_UNO_R2" H 5900 4140 50  0000 C CNN
F 2 "Module:Arduino_UNO_R2" H 5900 3050 50  0001 C CIN
F 3 "https://www.arduino.cc/en/Main/arduinoBoardUno" H 5900 3050 50  0001 C CNN
	1    5900 3050
	1    0    0    -1  
$EndComp
$Comp
L ventilator:MPRLS0025PA00001A U1
U 1 1 5E810603
P 10300 1400
F 0 "U1" H 10300 2081 50  0000 C CNN
F 1 "MPRLS0025PA00001A" H 10300 1990 50  0000 C CNN
F 2 "" H 11300 950 50  0001 C CNN
F 3 "https://sensing.honeywell.com/index.php?ci_id=172626" H 11450 1450 50  0001 C CNN
	1    10300 1400
	1    0    0    -1  
$EndComp
$Comp
L ventilator:MPRLS0025PA00001A U2
U 1 1 5E8111B8
P 10300 2750
F 0 "U2" H 10300 3431 50  0000 C CNN
F 1 "MPRLS0025PA00001A" H 10300 3340 50  0000 C CNN
F 2 "" H 11300 2300 50  0001 C CNN
F 3 "https://sensing.honeywell.com/index.php?ci_id=172626" H 11450 2800 50  0001 C CNN
	1    10300 2750
	1    0    0    -1  
$EndComp
$Comp
L ventilator:MPRLS0025PA00001A U3
U 1 1 5E811D8D
P 10300 4150
F 0 "U3" H 10300 4831 50  0000 C CNN
F 1 "MPRLS0025PA00001A" H 10300 4740 50  0000 C CNN
F 2 "" H 11300 3700 50  0001 C CNN
F 3 "https://sensing.honeywell.com/index.php?ci_id=172626" H 11450 4200 50  0001 C CNN
	1    10300 4150
	1    0    0    -1  
$EndComp
$Comp
L ventilator:MPRLS0025PA00001A U4
U 1 1 5E8127FE
P 10300 5550
F 0 "U4" H 10300 6231 50  0000 C CNN
F 1 "MPRLS0025PA00001A" H 10300 6140 50  0000 C CNN
F 2 "" H 11300 5100 50  0001 C CNN
F 3 "https://sensing.honeywell.com/index.php?ci_id=172626" H 11450 5600 50  0001 C CNN
	1    10300 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 5550 9550 5550
Wire Wire Line
	5400 3750 5400 5550
Wire Wire Line
	9900 4150 9550 4150
Wire Wire Line
	9550 4150 9550 5550
Connection ~ 9550 5550
Wire Wire Line
	9550 5550 9900 5550
Wire Wire Line
	9900 2750 9550 2750
Wire Wire Line
	9550 2750 9550 4150
Connection ~ 9550 4150
Wire Wire Line
	9550 2750 9550 1400
Wire Wire Line
	9550 1400 9900 1400
Connection ~ 9550 2750
$EndSCHEMATC
