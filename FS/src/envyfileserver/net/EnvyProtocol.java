/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package envyfileserver.net;

import envyfileserver.net.EnvyNetMessageProtos.EnvyNetMessage;

/**
 *
 * @author jithornton47
 */
public abstract class EnvyProtocol {
    protected final int PORT_NUMBER;
    protected boolean killConnection = false;
    
    public int currentState = 0;
    
    public EnvyProtocol(int port) {
        this.PORT_NUMBER = port;
    }
    
    public boolean shouldTerminate() { return this.killConnection; }
    public int port() { return this.PORT_NUMBER; }
    
    public abstract EnvyNetMessage processInput(String input);
}
