/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package CliTemplate;

import java.io.IOException;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.List;

import processing.app.debug.MessageConsumer;
import processing.app.debug.MessageSiphon;
import processing.app.debug.RunnerException;
import processing.app.helpers.ProcessUtils;

/**
 *
 * @author cousinr
 */
public class ExecCommand implements MessageConsumer {

  private boolean verbose = true;
  private boolean firstErrorFound;
  private boolean secondErrorFound;
  private RunnerException exception;

  /**
   * Either succeeds or throws a RunnerException fit for public consumption.
   * 
   * @param command
   * @throws RunnerException
   */
  public void execAsynchronously(String[] command) throws RunnerException {

    // eliminate any empty array entries
    List<String> stringList = new ArrayList<>();
    for (String string : command) {
      string = string.trim();
      if (string.length() != 0)
        stringList.add(string);
    }
    command = stringList.toArray(new String[stringList.size()]);
    if (command.length == 0)
      return;
    int result = 0;

    if (verbose) {
      for (String c : command)
        System.out.print(c + " ");
      System.out.println();
    }

    firstErrorFound = false;  // haven't found any errors yet
    secondErrorFound = false;

    Process process;
    try {
      process = ProcessUtils.exec(command);
    } catch (IOException e) {
      RunnerException re = new RunnerException(e.getMessage());
      re.hideStackTrace();
      throw re;
    }

    MessageSiphon in = new MessageSiphon(process.getInputStream(), this);
    MessageSiphon err = new MessageSiphon(process.getErrorStream(), this);

    // wait for the process to finish.  if interrupted
    // before waitFor returns, continue waiting
    boolean compiling = true;
    while (compiling) {
      try {
        in.join();
        err.join();
        result = process.waitFor();
        //System.out.println("result is " + result);
        compiling = false;
      } catch (InterruptedException ignored) { }
    }

    // an error was queued up by message(), barf this back to compile(),
    // which will barf it back to Editor. if you're having trouble
    // discerning the imagery, consider how cows regurgitate their food
    // to digest it, and the fact that they have five stomaches.
    //
    //System.out.println("throwing up " + exception);
    if (exception != null)
      throw exception;

    if (result > 1) {
      // a failure in the tool (e.g. unable to locate a sub-executable)
      System.err.println(MessageFormat.format("{0} returned {1}", command[0], result));
    }

    if (result != 0) {
      RunnerException re = new RunnerException(MessageFormat.format("exit code: {0}", result));
      re.hideStackTrace();
      throw re;
    }
  }

  /**
   * Part of the MessageConsumer interface, this is called
   * whenever a piece (usually a line) of error message is spewed
   * out from the compiler. The errors are parsed for their contents
   * and line number, which is then reported back to Editor.
     * @param s
   */
  @Override
  public void message(String s) {
    int i;

		
    System.err.print(s);
  }

}
