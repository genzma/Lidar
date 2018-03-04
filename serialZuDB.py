#!/usr/bin/python
import mysql.connector as mariadb
import serial
import random

#ser = serial.Serial('/dev/ttyACM0',115200)

mariadb_connection = mariadb.connect(user='pi', password='forscher', database='d')
cursor = mariadb_connection.cursor()

xMax = 70
yMax = 30

id = 1
i = 1
x = 1
y = 1
nr = 1

while True:
    try:
#       u = ser.readline().strip()
#       try:
#          w = int(w)
#       except ValueError:
#          w = 0

       w = random.randint(1,5000)
#       cursor.execute('UPDATE t SET w = \'' + str(random.randint(1,5000))  + '\', x = \''  + str(x)  + '\', y = \'' + str(y)  + '\', nr = \'' + str(nr) + '\'  WHERE id = \'' + str(id) + '\'')
       cursor.execute('INSERT INTO t (w, x, y, nr) VALUES ( \'' + str(w)  + '\', \'' + str(x)  + '\', \'' + str(y) +  '\', \''  + str(nr)  + '\');')

       mariadb_connection.commit()

#       print(y)

       x += 1
       if x > xMax:
          y += 1
          x = 1

       i += 1
       if i > xMax*yMax:
          i = 1
	  x = 1
          y = 1
	  nr += 1

       id += 1
       if id > 10500:
          sys.exit()



    except mariadb.Error as error:
       print("Error: {}".format(error));
    except KeyboardInterrupt:
       mariadb_connection.commit()
       sys.exit()


