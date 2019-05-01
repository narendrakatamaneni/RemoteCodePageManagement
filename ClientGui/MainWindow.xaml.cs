/////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - Code Publisher Client                      //
// ver 1.2                                                         //
// Narendra Katamaneni, CSE687 - Object Oriented Design            //
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
            ThreadStart thrdProc = () => {
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
        //----< load getFiles processing into dispatcher dictionary >------

        private void DispatcherLoadGetFiles()
        {
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

        private void DispatcherLoadConvert()
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

        private void DispatcherBrowse()
        {
            Action<CsMessage> browse = (CsMessage rcvMsg) =>
            {
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("browse"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            String path = Path.GetFullPath(workingdirectory.ToString()) + "\\" + file;
                            System.Diagnostics.Process.Start(path);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("file", browse);
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
            string[] args = Environment.GetCommandLineArgs();
            endPointPort = int.Parse(args[1]);
            serverPort= int.Parse(args[2]);
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
            txtRegex.Text = "[A-B](.*)";
            //pass the below one through run.bat
            pathStack_.Push("../../RemoteCodePageManagement");
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            translater.postMessage(msg);
            /*msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);

            /*CsMessage msgF = new CsMessage();
            msgF.add("to", CsEndPoint.toString(serverEndPoint));
            msgF.add("from", CsEndPoint.toString(endPoint_));
            msgF.add("command", "getFile");
            msgF.add("path", "CommLibWrapper.h");

            translater.postMessage(msgF);
            msgF.attributes["path"] = "CommLibWrapper.cpp";
            translater.postMessage(msgF);*/


            /*string[] args = Environment.GetCommandLineArgs();
            foreach (string arg in args)
            {
                if (arg == "/demo")
                {
                    startDemonstration();

                }
            }*/
            //CsMessage msgF = new CsMessage();
            //msgF.add("to", CsEndPoint.toString(serverEndPoint));
            //msgF.add("from", CsEndPoint.toString(endPoint_));
            //msgF.add("command", "getFile");
            //msgF.add("path", "CommLibWrapper.h");
            //translater.postMessage(msgF);
            //msgF.attributes["Path"] = "CommLibWrapper.cpp";
            //translater.postMessage(msgF);

        }

       /* void LoadNavTab(string path)
        {
            lstFiles.Items.Clear();
            //CurrPath.Text = path;
            string[] dirs = Directory.GetDirectories(path);
            lstFiles.Items.Add("..");
            foreach (string dir in dirs)
            {
                DirectoryInfo di = new DirectoryInfo(dir);
                string name = System.IO.Path.GetDirectoryName(dir);
                lstFiles.Items.Add(di.Name);
            }
          
        }*/
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


        /* private void btnPath_TextChanged(object sender, RoutedEventArgs e)
         {

             lstFiles.Items.Clear();
             lstFiles.Items.Add("..");
             var dirs = Directory.GetDirectories(WDirectory);
             foreach (var dir in dirs)
             {
                 string dirName
                 lstFiles.Items.Add(Path.GetFileName(dir));
             }
             List<string> filesMatchingPatterns = new List<string>();
             var patterns = Patterns.split(' ');
             foreach (var patt in patterns)
                 filesMatchingPatterns.AddRange(Directory.GetFiles(WDirectory, patt));
             else{
                 foreach (var file in filesMatchingPatterns) lstFiles.Items.Add(Path.GetFileName(file));
             }

         }*/
        //----< strip off name of first part of path >---------------------

        private string removeFirstDir(string path)
        {
            string modifiedPath = path;
            int pos = path.IndexOf("/");
            modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
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

      
        private void TxtPatterns_KeyUp(object sender, KeyEventArgs e)
        {

        }

        private void CvtdFiles_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {

            try
            {
                // build path for selected dir
                string selectedFile = (string)cvtdFiles.SelectedItem;
                if (selectedFile == null)
                    return;
                selectedFile=Path.GetFileName(selectedFile);
                // build message to get dirs and post it
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

            /*String str = cvtdFiles.SelectedItem.ToString();
            System.Diagnostics.Process.Start(str);*/
        }

        /*public bool test()
        {
            String[] commandLineArguments = Environment.GetCommandLineArgs();
            if (commandLineArguments.Length > 2)
            {
                string path = commandLineArguments[1];
                if (!Directory.Exists(path)) return false;
                WDirectory = Path.GetFullPath(path);
                txtPath.Text = WDirectory;

                for (int i = 2; i < commandLineArguments.Length; i++)
                {
                    string arg = commandLineArguments[i];
                    try
                    {
                        Regex r = new Regex(arg);
                        if (arg.Contains('['))
                            txtRegex.Text = arg;
                    }
                    catch (Exception)
                    {
                        txtPatterns.Text += " " + arg;
                    }
                }
                //BtnPublish_Click(this, new RoutedEventArgs());
            }
            else
            {
                Console.WriteLine("Invalid number of arguments. Arguments count should be 7");
                return false;
            }
            return true;
        }
        */
    }
}
