import serial
import serial.tools.list_ports
import time
import psutil
import sys

DEBUG = True

#function for connection to arduino
def connection():
    """Поиск Arduino на всех платформах"""
    ports = serial.tools.list_ports.comports()
    
    #названия для винды и линукс
    windows_patterns = ['CH340', 'Arduino', 'USB Serial Device']
    linux_patterns = ['CH340', 'USB Serial', 'ttyUSB', 'ttyACM']
    
    for port in ports: 
        port_info = f"{port.description} {port.device}".lower()
        
        # Для Windows
        if sys.platform.startswith('win'):
            for pattern in windows_patterns:
                if pattern.lower() in port_info:
                    print(f"Контроллер обнаружен на порту {port.device} ({port.description})")
                    return port.device
        # Для линукса 
        else:
            for pattern in linux_patterns:
                if pattern.lower() in port_info:
                    print(f"Контроллер обнаружен на порту {port.device} ({port.description})")
                    return port.device
    
    print("Arduino не найден автоматически. Доступные порты:")
    for port in ports:
        print(f"  {port.device} - {port.description}")
    
    #если автоматитка подвела
    if not sys.platform.startswith('win'):
        linux_ports = ['/dev/ttyUSB0', '/dev/ttyUSB1', '/dev/ttyACM0', '/dev/ttyACM1']
        for port in linux_ports:
            try:
                test_ser = serial.Serial(port, 9600, timeout=1)
                test_ser.close()
                print(f"Автоопределение: Arduino на {port}")
                return port
            except:
                continue
    
    return None

def get_system_info():
    # получение загружености ЦП и ОЗУ
    cpu_percent = psutil.cpu_percent(interval=1)
    ram_percent = psutil.virtual_memory().percent
    return cpu_percent, ram_percent

def send_to_arduino(ser, cpu, ram):
    #отправка на контроллер
    data_string = f"CPU:{cpu:.0f}%,RAM:{ram:.0f}%\n"
    ser.write(data_string.encode())
    if DEBUG:
        print(f"Sent: {data_string.strip()}")

def main():
    try:
        # Сохраняем порт в переменную (исправление двойного вызова)
        arduino_port = connection()
        
        if arduino_port:
            print(f"Подключаемся к {arduino_port}...")
            ser = serial.Serial(arduino_port, 9600, timeout=1)
        else:
            print("Ардуино не обнаружен")
            return 1
            
        time.sleep(2)  # Ожидание инициализации Arduino
        
        print("System monitor started. Press Ctrl+C to stop.")
        print(f"Платформа: {sys.platform}")
        
        while True:
            cpu, ram = get_system_info()
            send_to_arduino(ser, cpu, ram)
            time.sleep(2)  # Отправка каждые 2 секунды
    #обработка ошибок и ^C        
    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except PermissionError as e:
        print(f"Permission error: {e}")
        if not sys.platform.startswith('win'):
            print("Запустите с sudo: sudo python script.py")
    except KeyboardInterrupt:
        print("Program stopped by user")
        if 'ser' in locals():
            ser.close()
    except Exception as e:
        print(f"Unexpected error: {e}")

if __name__ == "__main__":
    main()
