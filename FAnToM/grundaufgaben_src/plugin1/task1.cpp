#include <fantom/algorithm.hpp>
#include <fantom/gui.hpp>
#include <fantom/register.hpp>
#include <mutex>

using namespace fantom;

namespace
{
    class WindowTutorialAlgorithm : public VisAlgorithm
    {
        class MyWindow : public DockWindow
        {
        public:
            MyWindow(MainWindow &mainWindow, const std::string &name)
                : DockWindow(mainWindow, DockWindow::FFREE, name), mLayout(*this, false), mText(mLayout), mButton(mLayout, "Send", std::bind(&MyWindow::send, this))
            {
            }

            void send()
            {
                if (mSendCallback)
                {
                    mSendCallback(mText.get());
                }
            }

            void setSendCallback(std::function<void(const std::string &)> callback)
            {
                mSendCallback = callback;
            }

        private:
            BoxLayout mLayout;
            LineEdit mText;
            PushButton mButton;

            std::function<void(const std::string &)> mSendCallback;
        };

        void print(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            infoLog() << name << std::endl;
        }

        std::mutex mMutex;

    public:
        class VisOutputs : public VisAlgorithm::VisOutputs
        {
        public:
            VisOutputs(Control &control)
                : VisAlgorithm::VisOutputs(control)
            {
                addWindow<MyWindow>("Algorithm Window");
            }
        };

        WindowTutorialAlgorithm(InitData &init)
            : VisAlgorithm(init)
        {
            getWindow<MyWindow>("Algorithm Window")
                .setSendCallback(std::bind(&WindowTutorialAlgorithm::print, this, std::placeholders::_1));
        }

        ~WindowTutorialAlgorithm()
        {
            getWindow<MyWindow>("Algorithm Window").setSendCallback(nullptr);
        }

        virtual void execute(const Algorithm::Options &, const volatile bool &)
        {
        }
    };

    AlgorithmRegister<WindowTutorialAlgorithm> dummy("Tutorial/Window", "Demonstrate algorithm windows.");
}