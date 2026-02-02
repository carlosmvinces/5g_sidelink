package simu5g.simulations.NR.examen_cuenca;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.networklayer.ipv4.RoutingTableRecorder;
import inet.visualizer.canvas.integrated.IntegratedCanvasVisualizer;

import simu5g.world.radio.LteChannelControl;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.common.binder.Binder;
import simu5g.nodes.NR.gNodeB;

import org.car2x.veins.subprojects.veins_inet.VeinsInetManager;

network Cuenca
{
    parameters:
        double playgroundSizeX @unit(m);
        double playgroundSizeY @unit(m);
        double playgroundSizeZ @unit(m);
    	int numCars = default(0);

        @display("bgb=1891,987");

    submodules:
		vizualizer: IntegratedCanvasVisualizer {
            @display("p=60,250");
        }

        routingRecorder: RoutingTableRecorder {
            @display("p=60,60;is=s");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=60,110");
        }

        veinsManager: VeinsInetManager {
            @display("p=60,170;is=s");
        }

        channelControl: LteChannelControl {
            @display("p=60,20;is=s");
        }

        binder: Binder {
            @display("p=60,140;is=s");
        }

        carrierAggregation: CarrierAggregation {
            @display("p=60,210;is=s");
        }

        gNodeB1: gNodeB {
            @display("p=740,640;is=vl");
        }
    	
    	car[numCars]: simu5g.nodes.cars.NRCar {
        	@display("i=vehicle/car");
    	}

		rsu: simu5g.nodes.cars.NRCar {
            @display("i=device/antennatower;is=l"); // Icono de torre para diferenciarlo
        }
    connections allowunconnected:
           
}
