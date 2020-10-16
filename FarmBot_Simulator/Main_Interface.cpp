#include "Main_Interface.h"
#include <iostream>

using namespace std;
using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]

int main()

{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    FarmbotSimulator::Main_Interface form;
    Application::Run(% form);
    return 0;
}
