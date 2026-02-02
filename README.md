# Simulación NR Sidelink V2X para aplicaciones de seguridad vehicular

Este repositorio contiene la implementación desarrollada en OMNeT++ para la simulación de un escenario vehicular basado en comunicación 5G NR Sidelink (PC5), orientado a aplicaciones de seguridad crítica en Sistemas de Transporte Inteligente (ITS), como el Frenado de Emergencia Autónomo (AEB).

La simulación integra movilidad vehicular realista, comunicación directa entre vehículos y lógica de aplicación para la diseminación de mensajes de alerta, permitiendo evaluar métricas de confiabilidad y latencia a nivel de aplicación.

---

## Estructura del repositorio

.
├── omnet/
│   ├── ned/                 # Definición del escenario y nodos OMNeT++
│   ├── ini/                 # Archivo de configuración omnetpp.ini
│   ├── apps/                # AlertSender, AlertReceiver, AlertRelay
│   └── simulations/         # Archivos de simulación
├── sumo/
│   ├── net.xml              # Red vial
│   ├── rou.xml              # Rutas vehiculares
│   ├── poly.xml             # Elementos gráficos
│   └── sumocfg              # Configuración de SUMO
└── README.md

---

## Herramientas y versiones utilizadas

- OMNeT++ 6.x
- Simu5G
- INET Framework
- Veins
- SUMO
- Sistema operativo GNU/Linux

Nota: Las versiones de los frameworks deben ser compatibles entre sí para garantizar la correcta ejecución de la simulación.

---

## Descripción del escenario

El escenario simula un entorno vehicular urbano en el cual un vehículo genera un evento de frenado de emergencia. Dicho evento es comunicado al resto de los nodos mediante comunicación NR Sidelink en Modo 2, sin dependencia de infraestructura celular para la asignación de recursos de radio.

Los nodos receptores procesan los mensajes de alerta y reaccionan de forma diferenciada según el tipo de mensaje recibido, modelando un comportamiento vehicular coherente con aplicaciones de seguridad vial.

---

## Ejecución de la simulación

1. Iniciar SUMO utilizando el archivo de configuración `.sumocfg`.
2. Ejecutar OMNeT++ y cargar la simulación correspondiente.
3. Verificar la correcta conexión entre OMNeT++ y SUMO mediante la interfaz TraCI.
4. Ejecutar la simulación desde OMNeT++ utilizando Qtenv o Cmdenv.

---

## Resultados

A partir de la simulación se obtienen métricas a nivel de aplicación, tales como:
- Packet Delivery Ratio (PDR) por nodo
- Función de distribución acumulada (CDF) del PDR
- Retardo extremo a extremo de los mensajes de alerta

Estas métricas permiten evaluar la confiabilidad y la latencia del sistema NR Sidelink en escenarios de seguridad vehicular.

---

## Uso académico

Este proyecto fue desarrollado con fines académicos. El código y los archivos de configuración pueden ser utilizados y modificados para propósitos educativos y de investigación, citando adecuadamente a los autores y a los frameworks empleados.
