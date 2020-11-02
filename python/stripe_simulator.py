#!/usr/bin/python


from PyQt5.QtWidgets import QWidget, QApplication
from PyQt5.QtGui import QPainter, QPen, QColor
from PyQt5.QtCore import Qt
import sys, random, math


class LEDStripe(QWidget):

    def __init__(self, pixel_number):
        super().__init__()

        self.colorArray = []
        self.pixel_hight = 180
        self.pixel_width = 10
        self.pixel_number = pixel_number
        self.initUI()

    def initUI(self):
        self.setGeometry(0, 75, 40+self.pixel_number*self.pixel_width, self.pixel_hight+10)
        self.setWindowTitle('LED Stripe')
        #p = self.palette()
        #p.setColor(self.backgroundRole(), Qt.darkGray)
        #self.setPalette(p)
        self.show()
        # screen width may be to small. After showing the window, the window is truncated it is to large
        min_height = self.getPixelPosition(self.pixel_number - 1)[1] + self.pixel_hight + 5
        self.resize(self.frameSize().width(), min_height)

    def update_stripe(self, colors):
        self.colorArray = colors
        self.repaint()

    def paintEvent(self, e):
        qp = QPainter()
        qp.begin(self)
        #self.drawPoints(qp)
        self.drawStripe(qp, self.colorArray)
        qp.end()


    def drawStripe(self, qp, colors):
        
        for i in range(0,len(colors)):
            pixel_pos = self.getPixelPosition(i)
            qp.fillRect(pixel_pos[0], pixel_pos[1], self.pixel_width, self.pixel_hight, QColor.fromRgb(colors[i][0],colors[i][1],colors[i][2]))
            
    def getPixelPosition(self, index):
        size = self.frameSize()
        line_spacing = 10
        line_hight = line_spacing + self.pixel_hight
        # window width is may be no multiple of the pixel width
        line_width = (math.floor(size.width()/self.pixel_width)-2)*self.pixel_width
        line_number = math.floor((index*self.pixel_width)/line_width)
        y = 5 + line_number*line_hight
        # use pixel width as spacer
        x = self.pixel_width + (index*self.pixel_width)%line_width
        return (x, y)


def main():
    app = QApplication(sys.argv)
    ex = LEDStripe(100)
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
