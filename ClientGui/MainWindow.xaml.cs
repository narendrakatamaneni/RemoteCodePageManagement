/////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - Code Publisher Client                      //
// ver 1.2                                                         //
// Narendra Katamaneni CSE687 - Object Oriented Design            //
/////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package defines one class MainWindow that provides tabs:
 * - Find Libs: Navigate through local directory to find files for testing
 *   - Shows local directories and files
 *   - Can navigate by double clicking directories
 * 
 * Required Files:
 * ---------------
 * MainWindow.xaml, MainWindow.xaml.cs
 * SelectionWindow.xaml, SelectionWindow.xaml.cs
 * Translater.h
 * 
 * Maintenance History:
 * -------------------
 * ver 1.0 : 28 Mar 2019
 * - first release
 * 
 */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using Path = System.IO.Path;
using MsgPassingCommunication;
using System.Threading;
using System.Diagnostics;
using System.Text.RegularExpressions;
using System.Windows.Forms;

namespace ClientGui
{
    ///////////////////////////////////////////////////////////////////
    // MainWindow class
    // - Provides navigation and selection to find libraries to test.
    // - Creates a popup window to handle selections.

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        private string WDirectory { get; set; }
        private string patterns { get; set; }
        private string regex { get; set; }
        string workingdirectory = "../../../clientFiles";
        int endPointPort ;
        int serverPort;

        private Stack<string> pathStack_ = new Stack<string>();
        private Translater translater;
        private CsEndPoint endPoint_;
        private Thread rcvThrd = null;
        private Dictionary<string, Action<CsMessage>> dispatcher_
          = new Dictionary<string, Action<CsMessage>>();
        //----< process incoming messages on child thread >----------------

        private void processMessages()
        {
            try
            {
                ThreadStart thrdProc = () =>
                {
                    while (true)
                    {
                        CsMessage msg = translater.getMessage();
                        if (msg.attributes.Count == 0)
                            continue;
                        string msgId = msg.value("command");
                        if (dispatcher_.ContainsKey(msgId))
                            dispatcher_[msgId].Invoke(msg);
                    }
                };
                rcvThrd = new Thread(thrdProc);
                rcvThrd.IsBackground = true;
                rcvThrd.Start();
            }
            catch (Exception)
            {

            }
        }
        //----< function dispatched by child thread to main thread >-------

        private void clearDirs()
        {
            lstFiles.Items.Clear();
        }
        //----< function dispatched by child thread to main thread >-------

        private void addDir(string dir)
        {
            lstFiles.Items.Add(dir);
        }
        //----< function dispatched by child thread to main thread >-------

        private void insertParent()
        {
            lstFiles.Items.Insert(0, "..");
        }
        //----< function dispatched by child thread to main thread >-------

        private void clearFiles()
        {
            cvtdFiles.Items.Clear();
        }
        //----< function dispatched by child thread to main thread >-------

        private void addFile(string file)
        {
            lstFiles.Items.Add(file);
        }
        private void addConvertedFile(string file)
        {
            cvtdFiles.Items.Add(file);
        }
        //----< add client processing for message with key >---------------

        private void addClientProc(string key, Action<CsMessage> clientProc)
        {
            dispatcher_[key] = clientProc;
        }
        //----< load getDirs processing into dispatcher dictionary >-------

        private void DispatcherLoadGetDirs()
        {
            try
            {
                Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
                {
                    Action clrDirs = () =>
                    {
                        clearDirs();
                    };
                    Dispatcher.Invoke(clrDirs, new Object[] { });
                    var enumer = rcvMsg.attributes.GetEnumerator();
                    while (enumer.MoveNext())
                    {
                        string key = enumer.Current.Key;
                        if (key.Contains("dir"))
                        {
                            Action<string> doDir = (string dir) =>
                            {
                                addDir(dir);
                            };
                            Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
                        }
                    }
                    Action insertUp = () =>
                    {
                        insertParent();
                    };
                    Dispatcher.Invoke(insertUp, new Object[] { });
                };
                addClientProc("getDirs", getDirs);
            }
            catch (Exception)
            {

            }
        }
        //----< load getFiles processing into dispatcher dictionary >------

        private void DispatcherLoadGetFiles()
        {
            try { 
            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
                {
                    clearFiles();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            addFile(file);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("getFiles", getFiles);
            }
            catch (Exception)
            {

            }

        }

        private void DispatcherLoadConvert()
        {
            try
            {
                Action<CsMessage> convert = (CsMessage rcvMsg) =>
                {
                    Action clrFiles = () =>
                    {
                        clearFiles();
                    };
                    Dispatcher.Invoke(clrFiles, new Object[] { });
                    var enumer = rcvMsg.attributes.GetEnumerator();
                    while (enumer.MoveNext())
                    {
                        string key = enumer.Current.Key;
                        if (key.Contains("file"))
                        {
                            Action<string> doFile = (string file) =>
                            {
                                addConvertedFile(file);
                            };
                            Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                        }
                    }
                };
                addClientProc("convert", convert);
            }
            catch (Exception)
            {

            }
        }

        private void DispatcherBrowse()
        {
            int count = 0;
            try
            {
                Action<CsMessage> browse = (CsMessage rcvMsg) =>
                {
                    var enumer = rcvMsg.attributes.GetEnumerator();
                    while (enumer.MoveNext())
                    {
                        string key = enumer.Current.Key;
                        if (key.Contains("browse")&&count<1)
                        {
                            Action<string> doFile = (string file) =>
                            {
                                String path = Path.GetFullPath(workingdirectory.ToString()) + "\\" + file;
                                System.Diagnostics.Process.Start(path);
                                ++count;
                            };
                            Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                        }
                    }
                };
                addClientProc("file", browse);
            }
            catch (Exception)
            {

            }
        }

        //----< load all dispatcher processing >---------------------------

        private void loadDispatcher()
        {
            DispatcherLoadGetDirs();
            DispatcherLoadGetFiles();
            DispatcherLoadConvert();
            DispatcherBrowse();
        }

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            try
            {
                testStub();

                /*string[] args = Environment.GetCommandLineArgs();
                endPointPort = int.Parse(args[1]);
                serverPort = int.Parse(args[2]);
                // start Comm
                endPoint_ = new CsEndPoint();
                endPoint_.machineAddress = "localhost";
                endPoint_.port = endPointPort;
                translater = new Translater();
                translater.listen(endPoint_, workingdirectory, workingdirectory);
                // start processing messages
                processMessages();
                // load dispatcher
                loadDispatcher();

                CsEndPoint serverEndPoint = new CsEndPoint();
                serverEndPoint.machineAddress = "localhost";
                serverEndPoint.port = serverPort;
                txtPath.Text = "RemoteCodePageManagement";
                txtPatterns.Text = "*.h *.cpp";
                txtRegex.Text = "[A-D](.*)";
                //pass the below one through run.bat
                pathStack_.Push("../../RemoteCodePageManagement");
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "getDirs");
                msg.add("path", pathStack_.Peek());
                translater.postMessage(msg);*/
            }
            catch (Exception)
            {
                Console.WriteLine("Exception occured while loading main window");
            }

        }
        private void BtnPublish_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                CsEndPoint serverEndPoint = new CsEndPoint();
                serverEndPoint.machineAddress = "localhost";
                serverEndPoint.port = serverPort;
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "convert");
                msg.add("path", txtPath.Text);
                msg.add("appName", "Project4HelpWPF.exe");
                if ((bool)cbRecurse.IsChecked)
                {
                    msg.add("recursive", "/s");
                }
                msg.add("state", "/demo");
                msg.add("patterns", txtPatterns.Text);
                msg.add("regex", txtRegex.Text);
                translater.postMessage(msg);
            }
            catch (Exception)
            {
                Console.WriteLine("Exception occured while clicking on the Publish button");
            }
        }

        //----< strip off name of first part of path >---------------------

        private string removeFirstDir(string path)
        {
            string modifiedPath = path;
            try
            {             
               int pos = path.IndexOf("/");
               modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
               
            }
            catch (Exception)
            {
                Console.WriteLine("Exception occured while removing directory");
            }
            return modifiedPath;
        }

        private void LstFiles_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            try
            {
                // build path for selected dir
                string selectedDir = (string)lstFiles.SelectedItem;
                if (selectedDir == null)
                    return;
                string path;
                if (selectedDir == "..")
                {
                    if (pathStack_.Count > 1)  // don't pop off "Storage"
                        pathStack_.Pop();
                    else
                        return;
                }
                else
                {
                    path = pathStack_.Peek() + "/" + selectedDir;
                    pathStack_.Push(path);
                }
                // display path in Dir TextBlcok
                txtPath.Text = removeFirstDir(pathStack_.Peek());

                // build message to get dirs and post it
                CsEndPoint serverEndPoint = new CsEndPoint();
                serverEndPoint.machineAddress = "localhost";
                serverEndPoint.port = serverPort;
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "getDirs");
                msg.add("path", pathStack_.Peek());
                translater.postMessage(msg);
                msg.remove("command");
                msg.add("command", "getFiles");
                translater.postMessage(msg);
                lstFiles.Items.Clear();

            }
            catch (Exception)
            {
                Console.WriteLine("Exception occured : you have clicked on the blank space");
            }
        }

        private void CvtdFiles_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {

            try
            {
                string selectedFile = (string)cvtdFiles.SelectedItem;
                if (selectedFile == null)
                    return;
                selectedFile=Path.GetFileName(selectedFile);
                CsEndPoint serverEndPoint = new CsEndPoint();
                serverEndPoint.machineAddress = "localhost";
                serverEndPoint.port = serverPort;
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("convertedFile",selectedFile);
                msg.add("command", "getFile");
                translater.postMessage(msg);

            }
            catch (Exception)
            {
                Console.WriteLine("Exception occured : you have clicked on the blank space");
            }
        }

        private void testStub()
        {

            string[] args = Environment.GetCommandLineArgs();
            endPointPort = int.Parse(args[1]);
            serverPort = int.Parse(args[2]);
            // start Comm
            endPoint_ = new CsEndPoint();
            endPoint_.machineAddress = "localhost";
            endPoint_.port = endPointPort;
            translater = new Translater();
            translater.listen(endPoint_, workingdirectory, workingdirectory);
            // start processing messages
            processMessages();
            // load dispatcher
            loadDispatcher();

            CsEndPoint serverEndPoint1 = new CsEndPoint();
            serverEndPoint1.machineAddress = "localhost";
            serverEndPoint1.port = serverPort;
            txtPath.Text = "RemoteCodePageManagement";
            txtPatterns.Text = "*.h *.cpp";
            txtRegex.Text = "[A-B](.*)";
            //pass the below one through run.bat
            pathStack_.Push("../../RemoteCodePageManagement");
            CsMessage msg1 = new CsMessage();
            msg1.add("to", CsEndPoint.toString(serverEndPoint1));
            msg1.add("from", CsEndPoint.toString(endPoint_));
            msg1.add("command", "getDirs");
            msg1.add("path", pathStack_.Peek());
            translater.postMessage(msg1);

            CsEndPoint serverEndPoint2 = new CsEndPoint();
            serverEndPoint2.machineAddress = "localhost";
            serverEndPoint2.port = serverPort;
            CsMessage msg2 = new CsMessage();
            msg2.add("to", CsEndPoint.toString(serverEndPoint2));
            msg2.add("from", CsEndPoint.toString(endPoint_));
            msg2.add("command", "convert");
            //msg2.add("path", txtPath.Text);
            msg2.add("path", "../RemoteCodePageManagement");
            msg2.add("appName", "Project4HelpWPF.exe");
            if ((bool)cbRecurse.IsChecked)
            {
                msg2.add("recursive", "/s");
            }
            msg2.add("state", "/demo");
            msg2.add("patterns", txtPatterns.Text);
            msg2.add("regex", txtRegex.Text);
            translater.postMessage(msg2);

        }

    }
}
