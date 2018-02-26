/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package envyfileserver.net;

import com.google.protobuf.ByteString;
import envyfileserver.LoginServerCore;
import envyfileserver.net.EnvyNetMessageProtos.*;

import envyfileserver.net.EnvyNetMessageProtos.EnvyNetMessage;
import envyfileserver.schema.User;

/**
 *
 * @author jithornton47
 */
public class EnvyAP extends EnvyProtocol {

    private enum AuthState {
        STATE_INIT(0),
        STATE_AUTH_REQ(1),
        STATE_AUTH_REC(2),
        STATE_LOGGED_IN(3);

        private int val;

        AuthState(int val) {
            this.val = val;
        }

        public int val() {
            return this.val;
        }
    };

    private String username;
    private String password;

    public EnvyAP() {
        super(2356);
        super.currentState = AuthState.STATE_INIT.val();
    }

    private String[] commands = {
        "logout",
        "versions",
        "inject"
    };
    private final String envyCursor = ("Envy:\\\\>");

    private byte[] getHelpString() {
        String ret = "Available commands:\n";
        for (String command : commands) {
            ret += (command + "\n");
        }
        ret += envyCursor;
        return ret.getBytes();
    }

    @Override
    public EnvyNetMessage processInput(String input) {
        EnvyNetMessage.Builder messageBuilder = EnvyNetMessage.newBuilder();
        switch(this.currentState) {
            case 0: {
                //ret = new EnvyNetMessage(EnvyNetMessage.NetMessageType.NET_OPEN_CONNECTION);
                messageBuilder.setType(EnvyNetMessage.NetMessageType.NET_OPEN_CONNECTION);
                this.currentState = AuthState.STATE_AUTH_REQ.val();
                break;
            }
            case 1: {
                //ret = new EnvyNetMessage(EnvyNetMessage.NetMessageType.NET_AUTH_MESSAGE);
                messageBuilder.setType(EnvyNetMessage.NetMessageType.NET_AUTH_MESSAGE);
                this.currentState = AuthState.STATE_AUTH_REC.val();
                break;
            }
            case 2: {
                //ret = new EnvyNetMessage(EnvyNetMessage.NetMessageType.NET_AUTH_RESPONSE);
                messageBuilder.setType(EnvyNetMessage.NetMessageType.NET_AUTH_RESPONSE);
                System.out.println("User is attempting to authenticate with information:" + input);
                byte[] data = ("Welcome to envycheat.cc").getBytes();
                messageBuilder.setData(ByteString.copyFrom(data));
                messageBuilder.setDataSize(data.length);
                break;
            }
            default: break;
        }
        return messageBuilder.build();
    }
}
