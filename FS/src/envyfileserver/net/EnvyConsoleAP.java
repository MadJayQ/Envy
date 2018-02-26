/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package envyfileserver.net;

import com.google.protobuf.ByteString;
import envyfileserver.LoginServerCore;
import envyfileserver.net.EnvyNetMessageProtos.EnvyNetMessage;
import envyfileserver.schema.User;

/**
 *
 * @author jithornton47
 */
public class EnvyConsoleAP extends EnvyProtocol {

    private enum AuthState {
        STATE_INIT(0),
        STATE_REQ_USER(1),
        STATE_RECV_USER(2),
        STATE_RECV_PASS(3),
        STATE_AUTH(4),
        STATE_LOGGED_IN(5);

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

    public EnvyConsoleAP() {
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

        byte[] ret = {'\0'};
        switch (this.currentState) {
            case 0: {
                ret = ("Welcome to the Envycheat.cc Authentication Protocol\n").getBytes();
                this.currentState = AuthState.STATE_REQ_USER.val();
                break;
            }
            case 1: {
                ret = ("Username: ").getBytes();
                currentState = AuthState.STATE_RECV_USER.val();
                break;
            }
            case 2: {
                username = input;
                ret = ("Password: ").getBytes();
                currentState = AuthState.STATE_RECV_PASS.val();
                break;
            }
            case 3: {
                password = input;
                currentState = AuthState.STATE_AUTH.val();
            }
            case 4: {
                User u = new User(username, password);
                if (u.authenticate(LoginServerCore.dbFileName)) {
                    System.out.println("USER: " + u.handle() + " HAS AUTHENTICATED WITH ACCESS LEVEL: " + u.role());
                    ret = ("ACCESS GRANTED!\nUse the \"help\" command to see help!\n" + envyCursor).getBytes();
                    currentState = AuthState.STATE_LOGGED_IN.val();
                } else {
                    ret = ("ACCESS DENIED!").getBytes();
                    this.killConnection = true;
                }
                break;
            }
            case 5: {
                System.out.println("User wishes to execute command: " + input);
                if (input.equals("logout")) {
                    ret = ("Goodbye.").getBytes();
                    this.killConnection = true;
                } else if (input.equals("help")) {
                    ret = getHelpString();
                } else {
                    ret = ("Invalid command: " + input + "\n" + envyCursor).getBytes();
                }
                break;
            }
            default:
                break;
        }
        
        return EnvyNetMessage.newBuilder().
                setData(ByteString.copyFrom(ret)).
                setDataSize(ret.length).
                setType(EnvyNetMessage.NetMessageType.NET_AUTH_MESSAGE).
                build();
    }
}
