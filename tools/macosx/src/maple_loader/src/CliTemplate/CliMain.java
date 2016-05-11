/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package CliTemplate;

import java.io.IOException;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import processing.app.Preferences;

//import processing.app.I18n;
import processing.app.helpers.ProcessUtils;

/**
 *
 * @author cousinr
 */
public class CliMain {


  /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
        String comPort = args[0];   //
        String altIf = args[1];     // 
        String usbID = args[2];     // "1EAF:0003";
        String binFile = args[3];   // bin file
        
        System.out.println("maple_loader v0.1");
        
        Preferences.set ("serial.port",comPort);
        Preferences.set ("serial.parity","N");
        Preferences.setInteger ("serial.databits", 8);
        Preferences.setInteger ("serial.debug_rate",9600);
        Preferences.setInteger ("serial.stopbits",1);
        
        Preferences.setInteger ("programDelay",1200);
        
        Preferences.set ("upload.usbID", usbID);
        Preferences.set ("upload.altID", altIf);
        Preferences.setBoolean ("upload.auto_reset", true);
        Preferences.setBoolean ("upload.verbose", false);
        
        //
        DFUUploader dfuUploader = new DFUUploader();
        try {
            //dfuUploader.uploadViaDFU(binFile);
            dfuUploader.uploadViaDFU(binFile);
        } catch (Exception e)
        {
            System.err.print (MessageFormat.format("an error occurred! {0}\n", e.getMessage()));
        }
    }
}
