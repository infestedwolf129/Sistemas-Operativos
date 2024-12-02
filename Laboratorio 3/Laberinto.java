import java.io.FileReader;
import java.io.BufferedReader;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveTask;

public class Laberinto {

    static class Coordenada {
        int x, y;

        Coordenada(int x, int y) {
            this.x = x;
            this.y = y;
        }

        @Override
        public String toString() {
            return "[" + x + "," + y + "]";
        }
    }

    static class Interseccion {
        Coordenada coordenada;
        List<Integer> procesosInvolucrados;
    
        public Interseccion(Coordenada coordenada, List<Integer> procesosInvolucrados) {
            this.coordenada = coordenada;
            this.procesosInvolucrados = procesosInvolucrados;
        }
    
        @Override
        public String toString() {
            return "P" + procesosInvolucrados +
                   "-"+ coordenada +
                    '}';
        }
    }
    

    // Variable estatica para indicar si la salida ha sido encontrada
    static volatile boolean salidaEncontradaThreads = false;
    static volatile boolean salidaEncontradaForkJoin = false;
        
    //Resolucion del laberinto con Threads
    static class BusquedaLaberintoThread extends Thread {
        private char[][] laberinto;
        private int x, y;
        private int IDproceso;
        private List<Coordenada> ruta;
        private List<Integer> procesos;
        List<Interseccion> intersecciones = new CopyOnWriteArrayList<>();

        public BusquedaLaberintoThread(char[][] laberinto, int x, int y, int IDproceso, List<Coordenada> ruta, List<Integer> procesos, List<Interseccion> intersecciones) {
            this.laberinto = laberinto;
            this.x = x;
            this.y = y;
            this.IDproceso = IDproceso;
            this.ruta = new ArrayList<>(ruta);
            this.ruta.add(new Coordenada(x, y));
            this.procesos = new ArrayList<>(procesos);
            this.intersecciones = intersecciones;
            if (!procesos.contains(IDproceso)) {
                this.procesos.add(IDproceso);
            }
        }

        @Override
        public void run() {
            if(salidaEncontradaThreads) return;
            if (laberinto[x][y] == 'S') { 
                System.out.println("P" + IDproceso + "-" + new Coordenada(x, y) + "- Salida\n" + "Procesos involucrados: " + procesos);
                System.out.println("Ruta: " + ruta);
                System.out.println("Intersecciones: " + intersecciones);
                salidaEncontradaThreads = true;
                return;
            }

            laberinto[x][y] = 'V'; // Marcar como visitado en la matriz

            List<BusquedaLaberintoThread> threads = new ArrayList<>();
            int caminos = 0;

            if (validar(x + 1, y)) caminos++;
            if (validar(x - 1, y)) caminos++;
            if (validar(x, y + 1)) caminos++;
            if (validar(x, y - 1)) caminos++;

            if (caminos > 1) { // Interseccion
                int contador = 0;
                List<Integer> procesosInvolucrados = new ArrayList<>();
                procesosInvolucrados.add(IDproceso);
                intersecciones.add(new Interseccion(new Coordenada(x, y), procesosInvolucrados));
                if (validar(x + 1, y)) {
                    threads.add(new BusquedaLaberintoThread(laberinto, x + 1, y, IDproceso + 1, ruta, procesos, intersecciones));   
                    contador++;
                }
                if (validar(x - 1, y)) {
                    threads.add(new BusquedaLaberintoThread(laberinto, x - 1, y, IDproceso + 1 + contador, ruta, procesos, intersecciones));
                    contador++;
                }
                if (validar(x, y + 1)) {
                    threads.add(new BusquedaLaberintoThread(laberinto, x, y + 1, IDproceso + 1 + contador, ruta, procesos, intersecciones));
                    contador++;
                }
                if (validar(x, y - 1)) {
                    threads.add(new BusquedaLaberintoThread(laberinto, x, y - 1, IDproceso + 1 + contador, ruta, procesos, intersecciones));
                }

                // Procesamiento de threads
                for (BusquedaLaberintoThread thread : threads) {
                    thread.start();
                }
                for (BusquedaLaberintoThread thread : threads) {
                    try {
                        thread.join();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            } else { // Continuar explorando
                if (validar(x + 1, y)) {
                    new BusquedaLaberintoThread(laberinto, x + 1, y, IDproceso, ruta, procesos,intersecciones).run();
                } else if (validar(x - 1, y)) {
                    new BusquedaLaberintoThread(laberinto, x - 1, y, IDproceso, ruta, procesos,intersecciones).run();
                } else if (validar(x, y + 1)) {
                    new BusquedaLaberintoThread(laberinto, x, y + 1, IDproceso, ruta, procesos,intersecciones).run();
                } else if (validar(x, y - 1)) {
                    new BusquedaLaberintoThread(laberinto, x, y - 1, IDproceso, ruta, procesos,intersecciones).run();
                } else {
                    System.out.println("P" + IDproceso + "-" + new Coordenada(x, y));
                }
            }
        }

        private boolean validar(int x, int y) {
            return (laberinto[x][y] == '1' || laberinto[x][y] == 'S');
        }
    }

    //Resolucion del laberinto con ForkJoinPool
    static class BusquedaLaberinto extends RecursiveTask<Boolean> {
        private static final long serialVersionUID = 1L;
        private char[][] laberinto;
        private int x, y;
        private int IDproceso;
        private List<Coordenada> ruta;
        private List<Integer> procesos;
        private List<Interseccion> intersecciones;

        public BusquedaLaberinto(char[][] laberinto, int x, int y, int IDproceso, List<Coordenada> ruta, List<Integer> procesos, List<Interseccion> intersecciones) {
            this.laberinto = laberinto;
            this.x = x;
            this.y = y;
            this.IDproceso = IDproceso;
            this.ruta = new ArrayList<>(ruta);
            this.ruta.add(new Coordenada(x, y));
            this.procesos = new ArrayList<>(procesos);
            this.intersecciones = intersecciones;
            if(!procesos.contains(IDproceso)){
                this.procesos.add(IDproceso);
            };
        }

        @Override
        protected Boolean compute() {
            if (salidaEncontradaForkJoin) return true;
            if (laberinto[x][y] == 'S') { 
                System.out.println("P" + IDproceso + "-" + new Coordenada(x, y) + "- Salida\n" + "Procesos involucrados: " + procesos);
                System.out.println("Ruta: " + ruta);
                System.out.println("Intersecciones: " + intersecciones);
                salidaEncontradaForkJoin = true;
                return true;
            }

            laberinto[x][y] = 'V'; // Marcar como visitado en la matriz

            List<BusquedaLaberinto> tasks = new ArrayList<>();
            int caminos = 0;

            if (validar(x + 1, y)) caminos++;
            if (validar(x - 1, y)) caminos++;
            if (validar(x, y + 1)) caminos++;
            if (validar(x, y - 1)) caminos++;

            if (caminos > 1) { // Interseccion
                int contador = 0;
                List<Integer> procesosInvolucrados = new ArrayList<>();
                procesosInvolucrados.add(IDproceso);
                intersecciones.add(new Interseccion(new Coordenada(x, y), procesosInvolucrados));
                if (validar(x + 1, y)) {
                    tasks.add(new BusquedaLaberinto(laberinto, x + 1, y, IDproceso + 1, ruta, procesos, intersecciones));
                    contador = contador +1;
                }
                if (validar(x - 1, y)) {
                    tasks.add(new BusquedaLaberinto(laberinto, x - 1, y, IDproceso + 1 + contador, ruta, procesos, intersecciones));
                    contador = contador +1;
                }
                if (validar(x, y + 1)) {
                    tasks.add(new BusquedaLaberinto(laberinto, x, y + 1, IDproceso + 1 + contador, ruta, procesos, intersecciones));
                    contador = contador +1;
                }
                if (validar(x, y - 1)) {
                    tasks.add(new BusquedaLaberinto(laberinto, x, y - 1, IDproceso + 1 + contador, ruta, procesos, intersecciones));
                }

                // Forks y joins
                for (BusquedaLaberinto task : tasks) {
                    task.fork();
                }
                for (BusquedaLaberinto task : tasks) {
                    if (task.join()) {
                        return true;
                    }
                }
            } else { // Continuar explorando
                if (validar(x + 1, y)) {
                    return new BusquedaLaberinto(laberinto, x + 1, y, IDproceso, ruta, procesos,intersecciones).compute();
                }
                if (validar(x - 1, y)) {
                    return new BusquedaLaberinto(laberinto, x - 1, y, IDproceso, ruta, procesos,intersecciones).compute();
                }
                if (validar(x, y + 1)) {
                    return new BusquedaLaberinto(laberinto, x, y + 1, IDproceso, ruta, procesos,intersecciones).compute();
                }
                if (validar(x, y - 1)) {
                    return new BusquedaLaberinto(laberinto, x, y - 1, IDproceso, ruta, procesos,intersecciones).compute();
                }

                System.out.println("P" + IDproceso + "-" + new Coordenada(x, y));
            }

            return false;
        }

        private boolean validar(int x, int y) {
            return (laberinto[x][y] == '1' || laberinto[x][y] == 'S');
        }
    }

    public static void main(String[] args) {
        String laberinto = "laberinto.txt";
        try {
            // Se lee el archivo
            FileReader fr = new FileReader(laberinto);
            BufferedReader br = new BufferedReader(fr);

            // Se lee la primera linea
            String linea = br.readLine();
            String[] dimensiones = linea.split("x");
            int filas = Integer.parseInt(dimensiones[0]);
            int columnas = Integer.parseInt(dimensiones[1]);

            // Se crea un arreglo de caracteres para almacenar el laberinto
            char[][] lab = new char[filas][columnas];

            // Se lee la segunda linea que contiene las coordenadas de inicio
            linea = br.readLine();
            int startX = Integer.parseInt(linea.substring(1, linea.indexOf(",")));
            int startY = Integer.parseInt(linea.substring(linea.indexOf(",") + 1, linea.indexOf("]")));

            // Se lee el laberinto
            for (int i = 0; i < filas; i++) {
                linea = br.readLine();
                linea = linea.replace(" ", "");
                for (int j = 0; j < columnas; j++) {
                    lab[i][j] = linea.charAt(j);
                }
            }

            // Se cierra el buffer de lectura
            br.close();
            fr.close();
            
            // Escoger el método de búsqueda
            boolean usarThreads = true; // Cambiar a false para usar ForkJoin
            List<Interseccion> intersecciones = new CopyOnWriteArrayList<>();
            
            if (usarThreads) {
                long startTime = System.currentTimeMillis();
                // Se inicia la exploración del laberinto con Threads
                BusquedaLaberintoThread task = new BusquedaLaberintoThread(lab, startX, startY, 1, new ArrayList<>(), new ArrayList<>(), intersecciones);
                task.start();
                task.join();
                long endTime = System.currentTimeMillis();
                System.out.println("Tiempo de ejecucion Threads: " + (endTime - startTime) + "ms");
            } else {
                long startTime = System.currentTimeMillis();
                // Se inicia la exploración del laberinto con ForkJoinPool
                try (ForkJoinPool pool = new ForkJoinPool()){
                    BusquedaLaberinto task = new BusquedaLaberinto(lab, startX, startY, 1, new ArrayList<>(), new ArrayList<>(),intersecciones);
                    pool.invoke(task);
                }
                long endTime = System.currentTimeMillis();
                System.out.println("Tiempo de ejecucion ForksJoin: " + (endTime - startTime) + "ms");
                
            }
            
        } catch (Exception e) {
            System.out.println("Error: " + e.getMessage());
        }
    }
}
