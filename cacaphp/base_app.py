from subprocess import Popen, PIPE, STDOUT
from logging import info, warning, error
from errno import ENOENT, ECHILD
from select import select
from time import time, sleep
import os

class BaseApplication:
    def __init__(self, program, args=None):
        self.program = program
        if args:
            self.args = list(args)
        else:
            self.args = []
        self.process = None
        self.exit_failure = None
        self.popen_args = {}
        self._readline_buffer = {
            'stdout': [],
            'stderr': [],
        }

    def pipeStdout(self):
        self.popen_args['stdout'] = PIPE
        self.popen_args['stderr'] = STDOUT

    def pipeStderr(self):
        self.popen_args['stderr'] = PIPE

    def nullStdout(self):
        self.popen_args['stdout'] = open('/dev/null', 'w')
        self.popen_args['stderr'] = STDOUT

    def start(self):
        tmp = os.getcwd()
        try:
            os.chdir("/tmp")
            args = [self.program] + self.args
            self.process = Popen(args, **self.popen_args)
        except OSError, err:
            if err[0] == ENOENT:
                raise RuntimeError("No such program: %s" % self.program)
            else:
                raise
        finally:
            os.chdir(tmp)

    def info(self, message):
        info(message)

    def warning(self, message):
        warning(message)

    def error(self, message):
        error(message)

    def _readline_timeout(self, stream, timeout):
        select_in = [stream.fileno()]
        ok = False
        line = []
        while True:
            before = time()
            ready = select(select_in, [], [], timeout)[0]
            timeout -= (time() - before)
            if not ready:
                break

            char = stream.read(1)
            line.append(char)
            if char == '\n':
                ok = True
                break
        return (ok, ''.join(line))

    def readline(self, timeout=0, stream="stdout"):
        """
        Read one line from specified stream ('stdout' by default).

        timeout argument:
        - 0 (default): non-blocking read
        - None: blocking read
        - (float value): read with specified timeout in second

        Return a string with new line or None if their is no data.

        Code based on this code:
           http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/440554
        """
        if not self.process:
            return None

        out = getattr(self.process, stream)
        if not out:
            raise RuntimeError("Stream %s of process %s is not a pipe" % (stream, self))

        if timeout is not None:
            ok, line = self._readline_timeout(out, timeout)
        else:
            line = out.readline()
            ok = (line is not None)
        if not ok:
            if line:
                self._readline_buffer[stream].append(line)
            return None
        if self._readline_buffer[stream]:
            previous = self._readline_buffer[stream]
            line = ''.join(self._readline_buffer[stream]) + line
            self._readline_buffer[stream] = []
        line = line.rstrip()
        return line

    def readlines(self, timeout=0, total_timeout=None, stream="stdout"):
        if total_timeout:
            stop = time() + total_timeout
        else:
            stop = None
        while True:
            if stop:
                timeout = stop - time()
            line = self.readline(timeout, stream)
            if stop:
                if line is not None:
                    yield line
                if stop < time():
                    break
            else:
                if line is None:
                    break
                yield line


    def processOutput(self):
        line = None
        for line in self.readlines():
            if "Corrupt image" in line:
                print "ERR: Corrupt"
                return False
            if "Improper image header" in line:
                print "ERR: Image header"
                return False
            if "Unexpected end-of-file" in line:
                print "ERR: EOF"
                return False
            if "Insufficient image data in file" in line:
                print "Data underflow"
                return False
#        if line:
            print "last output line> %r" % line
        return None

    def _wait(self, blocking):
        """
        Wait process end.
        Return (is_still_running, exit_status).
        """
        raise NotImplementedError()

    def wait(self, blocking=True):
        """
        Return True is the process is still running,
        False otherwise.
        """
        if not self.process:
            return False
        self.exit_failure = True
        self.exit_code = None
        is_running, status = self._wait(blocking)
        if is_running:
            return True

        # Log exit code
        self.processExit(status)
        self.displayExit(status)
        self.process = None
        return False

    def isRunning(self):
        return self.wait(False)

    def stop(self):
        """
        Stop the process: call interrupt() every two seconds,
        and then kill() after timeout (or on keyboard interrupt).
        """
        if not self.isRunning():
            return
        self.warning("stop()")

        # First interrupt() call
        self.interrupt()

        # Wait until process ends
        step = 1
        signal = False
        start_time = time()
        while self.isRunning():
            if 2.0 < (time() - start_time):
                signal = True
                start_time = time()
            if signal:
                signal = False
                step += 1
                if step <= 2:
                    self.interrupt()
                else:
                    self.kill()
            try:
                sleep(0.250)
            except KeyboardInterrupt:
                self.info("Interrupted (CTRL+C)")
                signal = True

    def interrupt(self):
        """
        Interrupt the process: ask it to stop.
        On UNIX, use SIGINT signal.
        """
        raise NotImplementedError()

    def kill(self):
        """
        Kill the process: stop it unconditionally
        On UNIX, use SIGKILL signal.
        On Windows, use TerminateProcess() function.
        """
        raise NotImplementedError()

    def processExit(self, status):
        """
        Function called on process exit.
        status is the result of waitpid()
        """
        pass

    def displayExit(self, status):
        """
        Application exited: display a message with the exit code.
        status is the result of waitpid(), may be None if application crashed.
        """
        self.warning("Exit")

