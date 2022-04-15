from datetime import datetime
import time

def logging():
  i = 0
  while True:
    datetime_obj = datetime.now()
    sec = datetime_obj.strftime("%S")   
    file.write(f'{sec}\t{i}\t{i+6}\t{i+2}\n')
    i = i+1
    time.sleep(1)

if __name__ == "__main__":
  
  try:
    datetime_obj = datetime.now()
    date_time = datetime_obj.strftime("%d-%m-%Y_%H-%M-%S")
    file = open(f'LOG_{date_time}.txt', 'w')
    file.write('CO2\tCO\tTEMP\n')
    logging()
  
  except KeyboardInterrupt:
    file.close()