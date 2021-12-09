// #include <fantom/algorithm.hpp>
// #include <fantom/gui.hpp>
// #include <fantom/register.hpp>
// #include <mutex>

// using namespace fantom;

// namespace
// {

//     class TaskEins : public VisAlgorithm
//     {

//         class MyWindow : public DockWindow
//         {
//         public:
//             MyWindow(MainWindow &mainWindow, const std::string &name)
//                 : DockWindow(mainWindow, DockWindow::FLEFT, name)
//                 , mLayout(*this, false)
//                 , mLabel(mLayout, "Anzahl der Häuser eingeben")
//                 , mText(mLayout)
//                 , mButton(mLayout, "Send", std::bind(&MyWindow::send, this))
//             {
//             }

//             void send()
//             {
//                 if (mSendCallback)
//                 {
//                     mSendCallback(mText.get());
//                 }
//             }

//             void setSendCallback(std::function<void(const std::string &)> callback)
//             {
//                 mSendCallback = callback;
//             }

//         private:
//             BoxLayout mLayout;
//             Label mLabel;
//             LineEdit mText;
//             PushButton mButton;

//             std::function<void(const std::string &)> mSendCallback;
//         };

//         void print(const std::string &name)
//         {
//             std::unique_lock<std::mutex> lock(mMutex);
//             infoLog() << name << std::endl;
//         }

//         std::mutex mMutex;

//     public:
//         class VisOutputs : public VisAlgorithm::VisOutputs
//         {
//         public:
//             VisOutputs(Control &control)
//                 : VisAlgorithm::VisOutputs(control)
//             {
//                 addWindow<MyWindow>("Algorithm Window");
//             }
//         };

//         TaskEins(InitData &init)
//             : VisAlgorithm(init)
//         {
//             getWindow<MyWindow>("Algorithm Window")
//                 .setSendCallback(std::bind(&TaskEins::print, this, std::placeholders::_1));
//         }

//         ~TaskEins()
//         {
//             getWindow<MyWindow>("Algorithm Window").setSendCallback(nullptr);
//         }

//         virtual void execute(const Algorithm::Options &, const volatile bool &)
//         {
//         }
//     };

//     AlgorithmRegister<TaskEins> dummy("Tasks/TaskEins", "Demonstrate algorithm windows.");
// }