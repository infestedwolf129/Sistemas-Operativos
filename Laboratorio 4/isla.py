import threading
import random as rand
from datetime import datetime
import time

rand.seed(39)

def archivos(nombres):
    for i in nombres:        
        try:
            open(f"{i}.txt", "w")
        except:
            pass

nombre_islas = [
    "Isla_de_las_Sombras",
    "Isla_del_Dragon_Rojo",
    "Isla_de_las_Estrellas",
    "Isla_del_Bosque_Encantado",
    "Isla_de_los_Susurros",
    "Isla_del_Trueno",
    "Isla_del_Tesoro_Oculto",
    "Salida",
    "Bar_del_Oraculo"
]

# Islas : (capacidad puente, duracion busqueda, capacidad de la isla)
islas = {
    "sombras": (20, 9, 10),
    "dragon": (8, 5, 2),
    "estrellas": (15, 7, 5),
    "bosque": (6, 4, 3),
    "susurros": (6, 1, 5),
    "trueno": (9, 4, 4),
    "tesoro_oculto": (7, 5, 2)
}

claves = {
    1: "sombras",
    2: "dragon",
    3: "estrellas",
    4: "bosque",
    5: "susurros",
    6: "trueno",
    7: "tesoro_oculto"
}

class Isla:

    def __init__(self, datos, nombre):
        self.nombre = nombre
        self.max_puente = threading.Semaphore(datos[0])
        self.max_isla = threading.Semaphore(datos[2])
        self.condicion = threading.Condition()
        self.lock = threading.Lock()
        self.actual_puente = 0
        self.isla_ocupada = 0
        self.tope_isla = datos[2]
        self.dentro_isla = 0
        self.busqueda = datos[1]

    def entrar_puente(self, pirata):
        time.sleep(0.5)
        self.max_puente.acquire()
        self.lock.acquire()
        tiempo = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        pirata[0] = f"{pirata[2][0]},{tiempo},"
        self.actual_puente += 1
        self.lock.release()
        with self.condicion:
            if self.actual_puente >= self.tope_isla and self.isla_ocupada == 0:
                self.condicion.notify()

        with self.condicion:
            self.condicion.wait()

        self.isla(pirata)

    def isla(self, pirata):
        self.max_isla.acquire()
        self.lock.acquire()
        self.actual_puente -= 1
        self.max_puente.release()
        self.dentro_isla += 1
        if self.dentro_isla == self.tope_isla:
            self.isla_ocupada = 1
        tiempo = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        with open(f"{self.nombre}.txt", "a") as archivo:
            archivo.write(f"{pirata[0]}{tiempo},{pirata[-1] + 1}\n")
        self.lock.release()
        time.sleep(self.busqueda)

        self.lock.acquire()
        pirata[-1] += 1
        self.dentro_isla -= 1
        self.lock.release()
        with self.condicion:
            if self.dentro_isla == 0:
                self.isla_ocupada = 0
                self.condicion.notify_all()
        self.max_isla.release()

    def last_call(self):
        with self.condicion:
            self.condicion.notify_all()

class BarOraculo:
    hilos_piratas = []

    def __init__(self):
        self.sombras = Isla(datos=islas['sombras'], nombre=nombre_islas[0])
        self.dragon = Isla(datos=islas['dragon'], nombre=nombre_islas[1])
        self.estrellas = Isla(datos=islas['estrellas'], nombre=nombre_islas[2])
        self.bosque = Isla(datos=islas['bosque'], nombre=nombre_islas[3])
        self.susurros = Isla(datos=islas['susurros'], nombre=nombre_islas[4])
        self.trueno = Isla(datos=islas['trueno'], nombre=nombre_islas[5])
        self.tesoro_oculto = Isla(datos=islas['tesoro_oculto'], nombre=nombre_islas[6])
        self.piratas = []
        self.condicion = threading.Condition()
        tiempo = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        self.max_piratas = 500
        self.actual_piratas = 0
        self.lock = threading.Lock()
        for i in range(1, self.max_piratas + 1):
            a = rand.randint(1, 7)
            b = rand.randint(1, 7)
            while a == b:
                b = rand.randint(1, 7)
            sample = ["", f"Pirata{i},{tiempo},", (f"Pirata{i}", a, b), 0]
            hilo = threading.Thread(target=self.puente_cola, args=(sample,))
            self.hilos_piratas.append(hilo)

    def ready(self):
        print("Iniciando programa")
        print("Ejecutando...")
        for i in self.hilos_piratas:
            i.start()

        for i in self.hilos_piratas:
            i.join()

        return

    def puente_cola(self, pirata):
        self.piratas.append(pirata)
        tiempo = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        self.lock.acquire()
        pirata[1] = f"{pirata[1]}{nombre_islas[int(pirata[2][1] - 1)]},{tiempo},"
        self.lock.release()
        isla = self.get_isla(pirata[2][1])
        isla.entrar_puente(pirata)
        self.lock.acquire()
        self.actual_piratas -= 1
        self.lock.release()
        self.puente_cola_2(pirata)

    def puente_cola_2(self, pirata):
        tiempo = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        self.lock.acquire()
        pirata[1] = f"{pirata[1]}{nombre_islas[int(pirata[2][2] - 1)]},{tiempo}"
        with open("Bar_del_Oraculo.txt", "a") as archivo:
            archivo.write(f"{pirata[1]}\n")
        self.lock.release()
        isla = self.get_isla(pirata[2][2])
        isla.entrar_puente(pirata)
        self.lock.acquire()
        self.actual_piratas -= 1
        self.lock.release()
        tiempo = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        self.lock.acquire()
        with open("Salida.txt", "a") as archivo:
            archivo.write(f"{pirata[2][0]},{tiempo}\n")
        self.piratas.pop()
        self.lock.release()
        if len(self.piratas) < 10:
            self.sombras.last_call()
            self.dragon.last_call()
            self.estrellas.last_call()
            self.bosque.last_call()
            self.susurros.last_call()
            self.trueno.last_call()
            self.tesoro_oculto.last_call()

    def get_isla(self, key):
        if key == 1:
            return self.sombras
        elif key == 2:
            return self.dragon
        elif key == 3:
            return self.estrellas
        elif key == 4:
            return self.bosque
        elif key == 5:
            return self.susurros
        elif key == 6:
            return self.trueno
        elif key == 7:
            return self.tesoro_oculto

archivos(nombre_islas)
bar = BarOraculo()
bar.ready()
print("Programa terminado")
