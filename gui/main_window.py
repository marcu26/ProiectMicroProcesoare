import sys
from PySide6.QtWidgets import QMainWindow, QHBoxLayout, QVBoxLayout, QWidget, QGroupBox, QLabel, QPushButton, QTextEdit
from PySide6.QtGui import QIcon, QFont
from PySide6.QtCore import Qt, QTimer
import pyqtgraph as pg
import serial

first_color = "background-color: rgb(218, 218, 224)"
second_color = "background-color: rgb(162, 162, 166)"

class MainWindow(QMainWindow):
    promotie: str = "2023-2024"
    team: list[str] = [
        "Sandu Dragos",
        "Toma Marcu",
    ]

    def __init__(self):
        super().__init__()
        self.setWindowTitle(f"Proiect Microprocesoare {self.promotie}")
        self.setWindowIcon(QIcon("./icon.png"))

        primary_layout = QVBoxLayout()
        secondary_layout = QHBoxLayout()
        tertiary_layout = QVBoxLayout()

        team_box = QGroupBox("Membrii echipei")
        bold_font = QFont()
        bold_font.setBold(True)
        team_box.setFont(bold_font)

        first_member = QLabel(f"Membru 1: {self.team[0]}")
        second_member = QLabel(f"Membru 2: {self.team[1]}")
        team_box_layout = QVBoxLayout()
        team_box_layout.addWidget(first_member, 1)
        team_box_layout.addWidget(second_member, 1)
        team_box.setLayout(team_box_layout)

        control_panel_box = QGroupBox("Control Panel")
        control_panel_box.setFont(bold_font)

        button1 = QPushButton("Toggle LEDs")
        button2 = QPushButton("Change LEDs direction")
        button3 = QPushButton("Toggle Buzzer")

        button1.setStyleSheet(first_color)
        button2.setStyleSheet(first_color)
        button3.setStyleSheet(first_color)

        self.button1 = button1
        self.button2 = button2
        self.button3 = button3

        self.button1_c = 1
        self.button2_c = 1
        self.button3_c = 1

        button1.clicked.connect(self.toggle_leds)
        button2.clicked.connect(self.invert_leds)
        button3.clicked.connect(self.toggle_buzzer)

        control_panel_box_layout = QVBoxLayout()
        control_panel_box_layout.setSpacing(5)
        control_panel_box_layout.addWidget(button1, 1)
        control_panel_box_layout.addWidget(button2, 1)
        control_panel_box_layout.addWidget(button3, 1)
        control_panel_box.setLayout(control_panel_box_layout)

        tertiary_layout.addWidget(team_box, 1)
        tertiary_layout.addWidget(control_panel_box, 5)


        #

        plot_widget = pg.PlotWidget()
        plot_widget.setBackground("black")
        plot_widget.showGrid(x=True, y=True, alpha=0.5)
        self.plot_data = [0]*20
        self.bar_plot = pg.BarGraphItem(x=range(1, 21), height=self.plot_data, width=0.9)
        plot_widget.addItem(self.bar_plot)


        secondary_layout.addWidget(plot_widget, 3)
        secondary_layout.addLayout(tertiary_layout, 1)

        primary_layout.addLayout(secondary_layout, 4)
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)

        debug_box = QGroupBox("Values")
        debug_box_layout = QVBoxLayout()
        debug_box_layout.addWidget(self.text_edit, 1)
        debug_box.setLayout(debug_box_layout)

        primary_layout.addWidget(debug_box, 1)

        widget = QWidget()
        widget.setLayout(primary_layout)

        self.setCentralWidget(widget)

        # Comunicarea seriala
        self.serial_port = serial.Serial()
        self.serial_port.baudrate = 9600
        self.serial_port.port = 'COM3'
        self.serial_port.open()

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.read_serial_data)
        self.timer.start(100)

    def toggle_leds(self):
        self.serial_port.write(b'a')
        self.serial_port.reset_output_buffer()
        if self.button1_c == 1:
            self.button1.setStyleSheet(second_color)
            self.button1_c = 0
        else:
            self.button1.setStyleSheet(first_color)
            self.button1_c = 1  

    def invert_leds(self):
        self.serial_port.write(b'b')
        self.serial_port.reset_output_buffer()
        if self.button2_c == 1:
            self.button2.setStyleSheet(second_color)
            self.button2_c = 0
        else:
            self.button2.setStyleSheet(first_color)
            self.button2_c = 1

    def toggle_buzzer(self):
        self.serial_port.write(b'c')
        self.serial_port.reset_output_buffer()
        if self.button3_c == 1:
            self.button3.setStyleSheet(second_color)
            self.button3_c = 0
        else:
            self.button3.setStyleSheet(first_color)
            self.button3_c = 1

    def read_serial_data(self):
        char = self.serial_port.read(1)
        if char == b'\r':
            bvalue = self.serial_port.read(4).decode()
            self.serial_port.reset_input_buffer()
            try:
                value = float(bvalue)
                self.plot_data.append(value)
                self.plot_data = self.plot_data[-20:]
                x_positions = range(1, len(self.plot_data) + 1)

                color_list = []
                for val in self.plot_data:
                    if val <= 0.5:
                        color_list.append('g')
                    elif 0.5 < val <= 3.0:
                        color_list.append('y')
                    else:
                        color_list.append('r')
                self.bar_plot.setOpts(x=x_positions, height=self.plot_data, width=0.9, brushes=color_list)
                self.text_edit.insertPlainText(f"Received data: {value}\n")
            except ValueError:
                self.text_edit.insertPlainText(f"Invalid data received: {bvalue}\n")
