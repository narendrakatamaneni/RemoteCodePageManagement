///////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - GUI for Project3HelpWPF                      //
// ver 1.0                                                           //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018         //
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a WPF-based GUI for Project3HelpWPF demo.  It's 
 * responsibilities are to:
 * - Provide a display of directory contents of a remote ServerPrototype.
 * - It provides a subdirectory list and a filelist for the selected directory.
 * - You can navigate into subdirectories by double-clicking on subdirectory
 *   or the parent directory, indicated by the name "..".
 *   
 * Required Files:
 * ---------------
 * Mainwindow.xaml, MainWindow.xaml.cs
 * Translater.dll
 * 
 * Maintenance History:
 * --------------------
 * ver 1/1 : 07 Aug 2018
 * - fixed bug in DirList_MouseDoubleClick by returning when selectedDir is null
 * ver 1.0 : 30 Mar 2018
 * - first release
 * - Several early prototypes were discussed in class. Those are all superceded
 *   by this package.
 */

// Translater has to be statically linked with CommLibWrapper
// - loader can't find Translater.dll dependent CommLibWrapper.dll
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
using System.Threading;
using MsgPassingCommunication;

namespace WpfApp1
{
  public partial class MainWindow : Window
  {
    string workingdirectory = "../../../../RemoteCodePageManagement";
    public MainWindow()
    {
      InitializeComponent();
    }

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
      DirList.Items.Clear();
    }
    //----< function dispatched by child thread to main thread >-------

    private void addDir(string dir)
    {
      DirList.Items.Add(dir);
    }
    //----< function dispatched by child thread to main thread >-------

    private void insertParent()
    {
      DirList.Items.Insert(0, "..");
    }
    //----< function dispatched by child thread to main thread >-------

    private void clearFiles()
    {
      FileList.Items.Clear();
    }
    //----< function dispatched by child thread to main thread >-------

    private void addFile(string file)
    {
      FileList.Items.Add(file);
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
    //----< load all dispatcher processing >---------------------------

    private void loadDispatcher()
    {
      DispatcherLoadGetDirs();
      DispatcherLoadGetFiles();
    }
    //----< start Comm, fill window display with dirs and files >------

    private void Window_Loaded(object sender, RoutedEventArgs e)
    {
      // start Comm
      endPoint_ = new CsEndPoint();
      endPoint_.machineAddress = "localhost";
      endPoint_.port = 8082;
      
      translater = new Translater();
      translater.listen(endPoint_,workingdirectory, workingdirectory);


      // start processing messages
      processMessages();

      // load dispatcher
      loadDispatcher();

      CsEndPoint serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = "localhost";
      serverEndPoint.port = 8080;
      PathTextBlock.Text = "RemoteCodePageManagement";
      //pass the below one through run.bat
      pathStack_.Push("../../RemoteCodePageManagement");
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathStack_.Peek());
      translater.postMessage(msg);
      msg.remove("command");
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


      string[] args = Environment.GetCommandLineArgs();
      foreach (string arg in args)
      {
        if (arg == "/demo")
        {
          startDemonstration();

        }
      }
      //CsMessage msgF = new CsMessage();
      //msgF.add("to", CsEndPoint.toString(serverEndPoint));
      //msgF.add("from", CsEndPoint.toString(endPoint_));
      //msgF.add("command", "getFile");
      //msgF.add("path", "CommLibWrapper.h");
      //translater.postMessage(msgF);
      //msgF.attributes["Path"] = "CommLibWrapper.cpp";
      //translater.postMessage(msgF);


    }

    private void startDemonstration()
    {
      //CsMessage msg = new CsMessage();
      //msg.add("to", CsEndPoint.toString(serverEndPoint));
      //msg.add("from", CsEndPoint.toString(endPoint_));
      //msg.add("command", "getFile");
      //msg.add("path", "CommLibWrapper.h");
    }

    //----< strip off name of first part of path >---------------------

    private string removeFirstDir(string path)
    {
      string modifiedPath = path;
      int pos = path.IndexOf("/");
      modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
      return modifiedPath;
    }
    //----< respond to mouse double-click on dir name >----------------

    private void DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
            try
            {
                // build path for selected dir
                string selectedDir = (string)DirList.SelectedItem;
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
                PathTextBlock.Text = removeFirstDir(pathStack_.Peek());

                // build message to get dirs and post it
                CsEndPoint serverEndPoint = new CsEndPoint();
                serverEndPoint.machineAddress = "localhost";
                serverEndPoint.port = 8080;
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "getDirs");
                msg.add("path", pathStack_.Peek());
                translater.postMessage(msg);

                // build message to get files and post it
                msg.remove("command");
                msg.add("command", "getFiles");
                translater.postMessage(msg);
                //msg.remove("command");
                //msg.add("command", "getFile");
                //translater.postMessage(msg);

            }
               catch (Exception)
            {
                Console.WriteLine("Exception occured : you have clicked on the blank space");
            }
    }

        private void FileList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            try
            {
                string selectedFile = (string)FileList.SelectedItem;
                CsEndPoint serverEndPoint = new CsEndPoint();
                serverEndPoint.machineAddress = "localhost";
                serverEndPoint.port = 8080;
                CsMessage msg = new CsMessage();
                msg.add("to", CsEndPoint.toString(serverEndPoint));
                msg.add("from", CsEndPoint.toString(endPoint_));
                msg.add("command", "convert");
                msg.add("filename", selectedFile);
                msg.add("filepath", PathTextBlock.Text);
                //msg.add("path", pathStack_.Peek());
                translater.postMessage(msg);

            }
            catch (Exception)
            {
                Console.WriteLine("Exception occured while mouse double click on FileList");
            }
        }
        //----< first test not completed >---------------------------------


    }
}
