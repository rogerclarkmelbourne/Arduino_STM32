/*
  DFUUploader - uploader implementation using DFU

  Copyright (c) 2010
  Andrew Meyer

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

package CliTemplate;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import processing.app.Preferences;
import processing.app.Serial;
import processing.app.debug.MessageConsumer;
import processing.app.debug.MessageSiphon;
import processing.app.debug.RunnerException;

/**
 *
 * @author bob
 */
public class DFUUploader implements MessageConsumer {

  boolean firstErrorFound;
  boolean secondErrorFound;
  // part of the PdeMessageConsumer interface
  boolean notFoundError;
  boolean verbose;
  RunnerException exception;

  static final String SUPER_BADNESS =
    "Compiler error!";

  public boolean uploadUsingPreferences(String binPath, boolean verbose)
  throws RunnerException {

    this.verbose = verbose;
    
    return uploadViaDFU(binPath);
  }

  // works with old and new versions of dfu-util
  private boolean found_device (String dfuData, String usbID)
  {
      return dfuData.contains(("Found DFU: [0x"+usbID.substring(0,4)).toUpperCase()) ||
              dfuData.contains(("Found DFU: ["+usbID.substring(0,4)).toUpperCase());              
  }
  
  public boolean uploadViaDFU (String binPath)
  throws RunnerException {
      
    this.verbose = Preferences.getBoolean ("upload.verbose");
              
    /* todo, check for size overruns! */
    String fileType="bin";
    
    if (fileType.equals("bin")) {
      String usbID = Preferences.get("upload.usbID");
      if (usbID == null) {
        /* fall back on default */
        /* this isnt great because is default Avrdude or dfu-util? */
        usbID = Preferences.get("upload.usbID");
      }

      /* if auto-reset, then emit the reset pulse on dtr/rts */
      if (Preferences.get("upload.auto_reset") != null) {
        if (Preferences.get("upload.auto_reset").toLowerCase().equals("true")) {
          System.out.println("Resetting to bootloader via DTR pulse");
          emitResetPulse();
        }
      } else {
        System.out.println("Resetting to bootloader via DTR pulse");
        emitResetPulse();
      }

      String dfuList = new String();
      List commandCheck = new ArrayList();
      commandCheck.add("dfu-util");
      commandCheck.add("-l");
      long startChecking = System.currentTimeMillis();
      System.out.println("Searching for DFU device [" + usbID + "]...");
      do {
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {}
        dfuList = executeCheckCommand(commandCheck);
        //System.out.println(dfuList);
      } 
      while ( !found_device (dfuList.toUpperCase(), usbID) && (System.currentTimeMillis() - startChecking < 7000));
      
      if ( !found_device (dfuList.toUpperCase(), usbID) ) 
      {
        System.out.println(dfuList);
        System.err.println("Couldn't find the DFU device: [" + usbID + "]");
        return false;
      }
      System.out.println("Found it!");

      /* todo, add handle to let user choose altIf at upload time! */
      String altIf = Preferences.get("upload.altID");

      List commandDownloader = new ArrayList();
      commandDownloader.add("dfu-util");
      commandDownloader.add("-a "+altIf);
      commandDownloader.add("-R");
      commandDownloader.add("-d "+usbID);
      commandDownloader.add("-D"+ binPath); //"./thisbin.bin");

      return executeUploadCommand(commandDownloader);
    }

    System.err.println("Only .bin files are supported at this time");
    return false;
  }

 /* we need to ensure both RTS and DTR are low to start,
     then pulse DTR on its own. This is the reset signal
     maple responds to
  */
  private void emitResetPulse() throws RunnerException {

      /* wait a while for the device to reboot */
    int programDelay = Preferences.getInteger("programDelay");

    try {
      Serial serialPort = new Serial();

      // try to toggle DTR/RTS (old scheme)
      serialPort.setRTS(false);
      serialPort.setDTR(false);
      serialPort.setDTR(true);
      try {
            Thread.sleep(50);
      } catch (InterruptedException e) {}
      serialPort.setDTR(false);

      // try magic number
      serialPort.setRTS(true);
      serialPort.setDTR(true);
      try {
            Thread.sleep(50);
      } catch (InterruptedException e) {}
      serialPort.setDTR(false);
      try {
            Thread.sleep(50);
      } catch (InterruptedException e) {}
      serialPort.write("1EAF");
      try {
            Thread.sleep(50);
      } catch (InterruptedException e) {}
      serialPort.dispose();

    } catch(Exception e) {
      System.err.println("Reset via USB Serial Failed! Did you select the right serial port?\nAssuming the board is in perpetual bootloader mode and continuing to attempt dfu programming...\n");
    }
  }
  
    protected String executeCheckCommand(Collection commandDownloader)
    throws RunnerException
  {
    firstErrorFound = false;  // haven't found any errors yet
    secondErrorFound = false;
    notFoundError = false;
    int result=0; // pre-initialized to quiet a bogus warning from jikes

    String userdir = System.getProperty("user.dir") + File.separator;
    String returnStr = new String();

    try {
      String[] commandArray = new String[commandDownloader.size()];
      commandDownloader.toArray(commandArray);

      String armBasePath;

      //armBasePath = new String(Base.getHardwarePath() + "/tools/arm/bin/");
      armBasePath = "";

      commandArray[0] = armBasePath + commandArray[0];

      if (verbose || Preferences.getBoolean("upload.verbose")) {
        for(int i = 0; i < commandArray.length; i++) {
          System.out.print(commandArray[i] + " ");
        }
        System.out.println();
      }

      Process process = Runtime.getRuntime().exec(commandArray);
      BufferedReader stdInput = new BufferedReader(new
            InputStreamReader(process.getInputStream()));
      BufferedReader stdError = new BufferedReader(new
            InputStreamReader(process.getErrorStream()));

      // wait for the process to finish.  if interrupted
      // before waitFor returns, continue waiting
      //
      boolean busy = true;
      while (busy) {
        try {
          result = process.waitFor();
          busy = false;
        } catch (InterruptedException intExc) {
        }
      }

      String s;
      while ((s = stdInput.readLine()) != null) {
        returnStr += s + "\n";
      }

      process.destroy();

      if(exception!=null) {
        exception.hideStackTrace();
        throw exception;
      }
      if(result!=0) return "Error!";
    } catch (Exception e) {
      e.printStackTrace();
    }
    //System.out.println("result2 is "+result);
    // if the result isn't a known, expected value it means that something
    // is fairly wrong, one possibility is that jikes has crashed.
    //
    if (exception != null) throw exception;

    if ((result != 0) && (result != 1 )) {
      exception = new RunnerException(SUPER_BADNESS);
    }

    return returnStr; // ? true : false;

  }

    // Need to overload this from Uploader to use the system-wide dfu-util
  protected boolean executeUploadCommand(Collection commandDownloader)
    throws RunnerException
  {
    firstErrorFound = false;  // haven't found any errors yet
    secondErrorFound = false;
    notFoundError = false;
    int result=0; // pre-initialized to quiet a bogus warning from jikes

    String userdir = System.getProperty("user.dir") + File.separator;

    try {
      String[] commandArray = new String[commandDownloader.size()];
      commandDownloader.toArray(commandArray);

      String armBasePath;

      //armBasePath = new String(Base.getHardwarePath() + "/tools/arm/bin/");
      armBasePath = "";

      commandArray[0] = armBasePath + commandArray[0];

      if (verbose || Preferences.getBoolean("upload.verbose")) {
        for(int i = 0; i < commandArray.length; i++) {
          System.out.print(commandArray[i] + " ");
        }
        System.out.println();
      }

      Process process = Runtime.getRuntime().exec(commandArray);
      new MessageSiphon(process.getInputStream(), this);
      new MessageSiphon(process.getErrorStream(), this);

      // wait for the process to finish.  if interrupted
      // before waitFor returns, continue waiting
      //
      boolean compiling = true;
      while (compiling) {
        try {
          result = process.waitFor();
          compiling = false;
        } catch (InterruptedException intExc) {
        }
      }
      if(exception!=null) {
        exception.hideStackTrace();
        throw exception;
      }
      if(result!=0)
        return false;
    } catch (Exception e) {
      e.printStackTrace();
    }
    //System.out.println("result2 is "+result);
    // if the result isn't a known, expected value it means that something
    // is fairly wrong, one possibility is that jikes has crashed.
    //
    if (exception != null) throw exception;

    if ((result != 0) && (result != 1 )) {
      exception = new RunnerException(SUPER_BADNESS);
      //editor.error(exception);
      //PdeBase.openURL(BUGS_URL);
      //throw new PdeException(SUPER_BADNESS);
    }

    return (result == 0); // ? true : false;

  }

  // deal with messages from dfu-util...
  public void message(String s) {

    if(s.indexOf("dfu-util - (C) ") != -1) { return; }
    if(s.indexOf("This program is Free Software and has ABSOLUTELY NO WARRANTY") != -1) { return; }

    if(s.indexOf("No DFU capable USB device found") != -1) {
      System.err.print(s);
      exception = new RunnerException("Problem uploading via dfu-util: No Maple found");
      return;
    }

    if(s.indexOf("Operation not perimitted") != -1) {
      System.err.print(s);
      exception = new RunnerException("Problem uploading via dfu-util: Insufficient privilages");
      return;
    }

    // else just print everything...
    System.out.print(s);
  }

}
