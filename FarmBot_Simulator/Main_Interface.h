#pragma once
#include "Farmbot.h"
#include <iostream>

namespace FarmbotSimulator {

	//includes

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;


	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class Main_Interface : public System::Windows::Forms::Form
	{
	public:
		delegate void SafeCallBack(System::Object^ obj);
		delegate void update_position_delegate();

		Main_Interface(void)
		{
			InitializeComponent();
			findPorts();
			InitializeVariables();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Main_Interface()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ComboBox^ comboBoxCom;
	private: System::Windows::Forms::ComboBox^ comboBoxBaud;
	protected:

	protected:

	private: System::Windows::Forms::Button^ buttonInit;
	private: System::Windows::Forms::Button^ buttonClose;






	private: System::IO::Ports::SerialPort^ serialPort1;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ label2;




	private: System::Windows::Forms::ProgressBar^ progressBar1;
	private: System::Windows::Forms::Label^ label5;

	private: System::Windows::Forms::GroupBox^ groupBox1;
	private: System::Windows::Forms::GroupBox^ groupBox2;



	private: System::Windows::Forms::Label^ label9;

	private: System::Windows::Forms::Label^ label11;

	private: System::Windows::Forms::Label^ label13;







	private: System::Windows::Forms::Label^ lblXY;
	private: System::Windows::Forms::Label^ lblXZ;




	private: System::ComponentModel::IContainer^ components;

	private: System::Windows::Forms::MenuStrip^ menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^ archivoToolStripMenuItem;
	private: System::Windows::Forms::RichTextBox^ commandHistory;

	private: Farmbot^ farmbot;
	private: System::Windows::Forms::ToolStripMenuItem^ parametrosToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ verParametrosToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ editarParametrosToolStripMenuItem;
	private: System::Windows::Forms::GroupBox^ groupBox3;

	private: System::Windows::Forms::Label^ label7;
	private: System::Windows::Forms::Label^ label6;
	private: System::Windows::Forms::Label^ label4;
	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::PictureBox^ pictureBox2;
	private: System::Windows::Forms::GroupBox^ Actuadores;
	private: System::Windows::Forms::GroupBox^ groupBox4;
	private: System::Windows::Forms::PictureBox^ pictureBox3;




	private: System::Windows::Forms::Label^ label17;
	private: System::Windows::Forms::Label^ label16;
	private: System::Windows::Forms::Label^ label15;
	private: System::Windows::Forms::Label^ label8;
private: System::Windows::Forms::PictureBox^ pictureBox1;
private: System::Windows::Forms::Label^ label19;
private: System::Windows::Forms::Label^ label18;
private: System::Windows::Forms::TrackBar^ humPot1;
private: System::Windows::Forms::TrackBar^ humPot2;
private: System::Windows::Forms::TrackBar^ humPot3;
private: System::Windows::Forms::TrackBar^ humPot4;




private: System::Windows::Forms::PictureBox^ ActPump;
private: System::Windows::Forms::PictureBox^ ActLed;
private: System::Windows::Forms::PictureBox^ ActValvle;
private: System::Windows::Forms::ComboBox^ comboBox1;
private: System::Windows::Forms::Label^ label12;
private: System::Windows::Forms::Label^ label10;
private: System::Windows::Forms::ProgressBar^ progressBar2;


private: System::Windows::Forms::ToolStripMenuItem^ pinesToolStripMenuItem;

	
// Farmbot Variables
	private: int x;
	private: int y;
	private: int z;
	private: int x0;
	private: int y0;
	private: int z0;
		   int x_act;
		   int y_act;
		   int z_act;
	private: bool Pump;
	private: bool Led;
	private: bool Valvle;
	bool finishThread;
	Thread^ newThread1;
	
	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(Main_Interface::typeid));
			this->comboBoxCom = (gcnew System::Windows::Forms::ComboBox());
			this->comboBoxBaud = (gcnew System::Windows::Forms::ComboBox());
			this->buttonInit = (gcnew System::Windows::Forms::Button());
			this->buttonClose = (gcnew System::Windows::Forms::Button());
			this->serialPort1 = (gcnew System::IO::Ports::SerialPort(this->components));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->lblXY = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->lblXZ = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->archivoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->parametrosToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->verParametrosToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->editarParametrosToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->pinesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->commandHistory = (gcnew System::Windows::Forms::RichTextBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->progressBar2 = (gcnew System::Windows::Forms::ProgressBar());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			this->Actuadores = (gcnew System::Windows::Forms::GroupBox());
			this->ActPump = (gcnew System::Windows::Forms::PictureBox());
			this->ActLed = (gcnew System::Windows::Forms::PictureBox());
			this->ActValvle = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->pictureBox3 = (gcnew System::Windows::Forms::PictureBox());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->humPot1 = (gcnew System::Windows::Forms::TrackBar());
			this->humPot2 = (gcnew System::Windows::Forms::TrackBar());
			this->humPot3 = (gcnew System::Windows::Forms::TrackBar());
			this->humPot4 = (gcnew System::Windows::Forms::TrackBar());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			this->groupBox3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox2))->BeginInit();
			this->Actuadores->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ActPump))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ActLed))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ActValvle))->BeginInit();
			this->groupBox4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->humPot1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->humPot2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->humPot3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->humPot4))->BeginInit();
			this->SuspendLayout();
			// 
			// comboBoxCom
			// 
			this->comboBoxCom->FormattingEnabled = true;
			this->comboBoxCom->Location = System::Drawing::Point(145, 24);
			this->comboBoxCom->Margin = System::Windows::Forms::Padding(2);
			this->comboBoxCom->Name = L"comboBoxCom";
			this->comboBoxCom->Size = System::Drawing::Size(92, 21);
			this->comboBoxCom->TabIndex = 0;
			this->comboBoxCom->Text = L"COM1";
			// 
			// comboBoxBaud
			// 
			this->comboBoxBaud->FormattingEnabled = true;
			this->comboBoxBaud->Items->AddRange(gcnew cli::array< System::Object^  >(2) { L"9600", L"115200" });
			this->comboBoxBaud->Location = System::Drawing::Point(145, 84);
			this->comboBoxBaud->Margin = System::Windows::Forms::Padding(2);
			this->comboBoxBaud->Name = L"comboBoxBaud";
			this->comboBoxBaud->Size = System::Drawing::Size(92, 21);
			this->comboBoxBaud->TabIndex = 1;
			this->comboBoxBaud->Text = L"115200";
			// 
			// buttonInit
			// 
			this->buttonInit->Location = System::Drawing::Point(4, 220);
			this->buttonInit->Margin = System::Windows::Forms::Padding(2);
			this->buttonInit->Name = L"buttonInit";
			this->buttonInit->Size = System::Drawing::Size(90, 41);
			this->buttonInit->TabIndex = 2;
			this->buttonInit->Text = L"Init Farmbot Port";
			this->buttonInit->UseVisualStyleBackColor = true;
			this->buttonInit->Click += gcnew System::EventHandler(this, &Main_Interface::buttonInit_Click);
			// 
			// buttonClose
			// 
			this->buttonClose->Location = System::Drawing::Point(145, 220);
			this->buttonClose->Margin = System::Windows::Forms::Padding(2);
			this->buttonClose->Name = L"buttonClose";
			this->buttonClose->Size = System::Drawing::Size(90, 41);
			this->buttonClose->TabIndex = 3;
			this->buttonClose->Text = L"Close Port";
			this->buttonClose->UseVisualStyleBackColor = true;
			this->buttonClose->Click += gcnew System::EventHandler(this, &Main_Interface::buttonClose_Click);
			// 
			// serialPort1
			// 
			this->serialPort1->PortName = L"COM3";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 30);
			this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(88, 13);
			this->label1->TabIndex = 6;
			this->label1->Text = L"Farmbot ComPort";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(6, 89);
			this->label2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(58, 13);
			this->label2->TabIndex = 7;
			this->label2->Text = L"Baud Rate";
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(4, 145);
			this->progressBar1->Margin = System::Windows::Forms::Padding(2);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(231, 19);
			this->progressBar1->TabIndex = 12;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(4, 124);
			this->label5->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(100, 13);
			this->label5->TabIndex = 13;
			this->label5->Text = L"Farmbot Port Status";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->lblXY);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->label7);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Location = System::Drawing::Point(206, 32);
			this->groupBox1->Margin = System::Windows::Forms::Padding(2);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Padding = System::Windows::Forms::Padding(2);
			this->groupBox1->Size = System::Drawing::Size(375, 406);
			this->groupBox1->TabIndex = 15;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L" X-Y Axis";
			// 
			// lblXY
			// 
			this->lblXY->BackColor = System::Drawing::Color::Black;
			this->lblXY->Location = System::Drawing::Point(0, 15);
			this->lblXY->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->lblXY->Name = L"lblXY";
			this->lblXY->Size = System::Drawing::Size(15, 16);
			this->lblXY->TabIndex = 0;
			// 
			// label3
			// 
			this->label3->BackColor = System::Drawing::Color::RosyBrown;
			this->label3->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label3->Location = System::Drawing::Point(22, 24);
			this->label3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(150, 163);
			this->label3->TabIndex = 1;
			// 
			// label7
			// 
			this->label7->BackColor = System::Drawing::Color::RosyBrown;
			this->label7->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label7->Location = System::Drawing::Point(202, 24);
			this->label7->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(150, 163);
			this->label7->TabIndex = 4;
			// 
			// label4
			// 
			this->label4->BackColor = System::Drawing::Color::RosyBrown;
			this->label4->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label4->Location = System::Drawing::Point(202, 220);
			this->label4->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(150, 163);
			this->label4->TabIndex = 2;
			// 
			// label6
			// 
			this->label6->BackColor = System::Drawing::Color::RosyBrown;
			this->label6->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label6->Location = System::Drawing::Point(22, 219);
			this->label6->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(150, 163);
			this->label6->TabIndex = 3;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->label19);
			this->groupBox2->Controls->Add(this->label18);
			this->groupBox2->Controls->Add(this->lblXZ);
			this->groupBox2->Location = System::Drawing::Point(206, 442);
			this->groupBox2->Margin = System::Windows::Forms::Padding(2);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Padding = System::Windows::Forms::Padding(2);
			this->groupBox2->Size = System::Drawing::Size(375, 81);
			this->groupBox2->TabIndex = 16;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Eje X-Z";
			// 
			// label19
			// 
			this->label19->BackColor = System::Drawing::Color::RosyBrown;
			this->label19->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label19->Location = System::Drawing::Point(202, 44);
			this->label19->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(150, 33);
			this->label19->TabIndex = 6;
			// 
			// label18
			// 
			this->label18->BackColor = System::Drawing::Color::RosyBrown;
			this->label18->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label18->Location = System::Drawing::Point(22, 44);
			this->label18->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(150, 33);
			this->label18->TabIndex = 5;
			// 
			// lblXZ
			// 
			this->lblXZ->BackColor = System::Drawing::Color::Black;
			this->lblXZ->Location = System::Drawing::Point(0, 15);
			this->lblXZ->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->lblXZ->Name = L"lblXZ";
			this->lblXZ->Size = System::Drawing::Size(15, 16);
			this->lblXZ->TabIndex = 0;
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(25, 107);
			this->label9->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(49, 13);
			this->label9->TabIndex = 20;
			this->label9->Text = L"Led Strip";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(28, 177);
			this->label11->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(34, 13);
			this->label11->TabIndex = 22;
			this->label11->Text = L"Valve";
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(17, 37);
			this->label13->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(76, 13);
			this->label13->TabIndex = 24;
			this->label13->Text = L"Vacuum Pump";
			// 
			// menuStrip1
			// 
			this->menuStrip1->ImageScalingSize = System::Drawing::Size(20, 20);
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->archivoToolStripMenuItem,
					this->parametrosToolStripMenuItem, this->pinesToolStripMenuItem
			});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Padding = System::Windows::Forms::Padding(4, 2, 0, 2);
			this->menuStrip1->RenderMode = System::Windows::Forms::ToolStripRenderMode::Professional;
			this->menuStrip1->Size = System::Drawing::Size(830, 24);
			this->menuStrip1->TabIndex = 30;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// archivoToolStripMenuItem
			// 
			this->archivoToolStripMenuItem->Name = L"archivoToolStripMenuItem";
			this->archivoToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->archivoToolStripMenuItem->Text = L"File";
			// 
			// parametrosToolStripMenuItem
			// 
			this->parametrosToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->verParametrosToolStripMenuItem,
					this->editarParametrosToolStripMenuItem
			});
			this->parametrosToolStripMenuItem->Name = L"parametrosToolStripMenuItem";
			this->parametrosToolStripMenuItem->Size = System::Drawing::Size(78, 20);
			this->parametrosToolStripMenuItem->Text = L"Parameters";
			// 
			// verParametrosToolStripMenuItem
			// 
			this->verParametrosToolStripMenuItem->Name = L"verParametrosToolStripMenuItem";
			this->verParametrosToolStripMenuItem->Size = System::Drawing::Size(167, 22);
			this->verParametrosToolStripMenuItem->Text = L"Ver Parametros";
			// 
			// editarParametrosToolStripMenuItem
			// 
			this->editarParametrosToolStripMenuItem->Name = L"editarParametrosToolStripMenuItem";
			this->editarParametrosToolStripMenuItem->Size = System::Drawing::Size(167, 22);
			this->editarParametrosToolStripMenuItem->Text = L"Editar Parametros";
			// 
			// pinesToolStripMenuItem
			// 
			this->pinesToolStripMenuItem->Name = L"pinesToolStripMenuItem";
			this->pinesToolStripMenuItem->Size = System::Drawing::Size(41, 20);
			this->pinesToolStripMenuItem->Text = L"Pins";
			// 
			// commandHistory
			// 
			this->commandHistory->Location = System::Drawing::Point(4, 275);
			this->commandHistory->Margin = System::Windows::Forms::Padding(2);
			this->commandHistory->Name = L"commandHistory";
			this->commandHistory->Size = System::Drawing::Size(232, 159);
			this->commandHistory->TabIndex = 31;
			this->commandHistory->Text = L"Command History";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->comboBox1);
			this->groupBox3->Controls->Add(this->label12);
			this->groupBox3->Controls->Add(this->label10);
			this->groupBox3->Controls->Add(this->progressBar2);
			this->groupBox3->Controls->Add(this->pictureBox1);
			this->groupBox3->Controls->Add(this->commandHistory);
			this->groupBox3->Controls->Add(this->label5);
			this->groupBox3->Controls->Add(this->buttonInit);
			this->groupBox3->Controls->Add(this->progressBar1);
			this->groupBox3->Controls->Add(this->buttonClose);
			this->groupBox3->Controls->Add(this->comboBoxBaud);
			this->groupBox3->Controls->Add(this->comboBoxCom);
			this->groupBox3->Controls->Add(this->label1);
			this->groupBox3->Controls->Add(this->label2);
			this->groupBox3->Location = System::Drawing::Point(586, 32);
			this->groupBox3->Margin = System::Windows::Forms::Padding(2);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Padding = System::Windows::Forms::Padding(2);
			this->groupBox3->Size = System::Drawing::Size(240, 491);
			this->groupBox3->TabIndex = 32;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Serial Communication";
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Location = System::Drawing::Point(145, 53);
			this->comboBox1->Margin = System::Windows::Forms::Padding(2);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(92, 21);
			this->comboBox1->TabIndex = 37;
			this->comboBox1->Text = L"COM2";
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(6, 58);
			this->label12->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(75, 13);
			this->label12->TabIndex = 36;
			this->label12->Text = L"Client ComPair";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(4, 167);
			this->label10->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(88, 13);
			this->label10->TabIndex = 35;
			this->label10->Text = L"Client Port Status";
			// 
			// progressBar2
			// 
			this->progressBar2->BackColor = System::Drawing::SystemColors::Control;
			this->progressBar2->Location = System::Drawing::Point(4, 189);
			this->progressBar2->Margin = System::Windows::Forms::Padding(2);
			this->progressBar2->Name = L"progressBar2";
			this->progressBar2->Size = System::Drawing::Size(231, 19);
			this->progressBar2->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
			this->progressBar2->TabIndex = 34;
			// 
			// pictureBox1
			// 
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(77, 437);
			this->pictureBox1->Margin = System::Windows::Forms::Padding(2);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(91, 49);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox1->TabIndex = 33;
			this->pictureBox1->TabStop = false;
			// 
			// pictureBox2
			// 
			this->pictureBox2->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox2.Image")));
			this->pictureBox2->Location = System::Drawing::Point(4, 32);
			this->pictureBox2->Margin = System::Windows::Forms::Padding(2);
			this->pictureBox2->Name = L"pictureBox2";
			this->pictureBox2->Size = System::Drawing::Size(197, 103);
			this->pictureBox2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox2->TabIndex = 33;
			this->pictureBox2->TabStop = false;
			// 
			// Actuadores
			// 
			this->Actuadores->Controls->Add(this->ActPump);
			this->Actuadores->Controls->Add(this->ActLed);
			this->Actuadores->Controls->Add(this->ActValvle);
			this->Actuadores->Controls->Add(this->label13);
			this->Actuadores->Controls->Add(this->label11);
			this->Actuadores->Controls->Add(this->label9);
			this->Actuadores->Location = System::Drawing::Point(5, 141);
			this->Actuadores->Margin = System::Windows::Forms::Padding(2);
			this->Actuadores->Name = L"Actuadores";
			this->Actuadores->Padding = System::Windows::Forms::Padding(2);
			this->Actuadores->Size = System::Drawing::Size(196, 214);
			this->Actuadores->TabIndex = 34;
			this->Actuadores->TabStop = false;
			this->Actuadores->Text = L"Actuators";
			// 
			// ActPump
			// 
			this->ActPump->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"ActPump.Image")));
			this->ActPump->Location = System::Drawing::Point(134, 16);
			this->ActPump->Margin = System::Windows::Forms::Padding(2);
			this->ActPump->Name = L"ActPump";
			this->ActPump->Size = System::Drawing::Size(52, 57);
			this->ActPump->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->ActPump->TabIndex = 28;
			this->ActPump->TabStop = false;
			// 
			// ActLed
			// 
			this->ActLed->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"ActLed.Image")));
			this->ActLed->Location = System::Drawing::Point(134, 81);
			this->ActLed->Margin = System::Windows::Forms::Padding(2);
			this->ActLed->Name = L"ActLed";
			this->ActLed->Size = System::Drawing::Size(52, 57);
			this->ActLed->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->ActLed->TabIndex = 27;
			this->ActLed->TabStop = false;
			// 
			// ActValvle
			// 
			this->ActValvle->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"ActValvle.Image")));
			this->ActValvle->Location = System::Drawing::Point(134, 153);
			this->ActValvle->Margin = System::Windows::Forms::Padding(2);
			this->ActValvle->Name = L"ActValvle";
			this->ActValvle->Size = System::Drawing::Size(52, 57);
			this->ActValvle->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->ActValvle->TabIndex = 26;
			this->ActValvle->TabStop = false;
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->pictureBox3);
			this->groupBox4->Controls->Add(this->label17);
			this->groupBox4->Controls->Add(this->label16);
			this->groupBox4->Controls->Add(this->label15);
			this->groupBox4->Controls->Add(this->label8);
			this->groupBox4->Controls->Add(this->humPot1);
			this->groupBox4->Controls->Add(this->humPot2);
			this->groupBox4->Controls->Add(this->humPot3);
			this->groupBox4->Controls->Add(this->humPot4);
			this->groupBox4->Location = System::Drawing::Point(4, 355);
			this->groupBox4->Margin = System::Windows::Forms::Padding(2);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Padding = System::Windows::Forms::Padding(2);
			this->groupBox4->Size = System::Drawing::Size(197, 168);
			this->groupBox4->TabIndex = 35;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Hygrometer";
			// 
			// pictureBox3
			// 
			this->pictureBox3->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox3.Image")));
			this->pictureBox3->Location = System::Drawing::Point(61, 123);
			this->pictureBox3->Margin = System::Windows::Forms::Padding(2);
			this->pictureBox3->Name = L"pictureBox3";
			this->pictureBox3->Size = System::Drawing::Size(75, 41);
			this->pictureBox3->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox3->TabIndex = 1;
			this->pictureBox3->TabStop = false;
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Location = System::Drawing::Point(107, 70);
			this->label17->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(72, 13);
			this->label17->TabIndex = 3;
			this->label17->Text = L"Farming Pot 4";
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Location = System::Drawing::Point(14, 70);
			this->label16->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(72, 13);
			this->label16->TabIndex = 2;
			this->label16->Text = L"Farming Pot 3";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(112, 20);
			this->label15->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(72, 13);
			this->label15->TabIndex = 1;
			this->label15->Text = L"Farming Pot 2";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(14, 20);
			this->label8->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(72, 13);
			this->label8->TabIndex = 0;
			this->label8->Text = L"Farming Pot 1";
			// 
			// humPot1
			// 
			this->humPot1->Location = System::Drawing::Point(10, 38);
			this->humPot1->Margin = System::Windows::Forms::Padding(2);
			this->humPot1->Maximum = 1023;
			this->humPot1->Name = L"humPot1";
			this->humPot1->Size = System::Drawing::Size(78, 45);
			this->humPot1->TabIndex = 4;
			this->humPot1->TickFrequency = 100;
			this->humPot1->Scroll += gcnew System::EventHandler(this, &Main_Interface::humPot1_Scroll);
			// 
			// humPot2
			// 
			this->humPot2->Location = System::Drawing::Point(110, 38);
			this->humPot2->Margin = System::Windows::Forms::Padding(2);
			this->humPot2->Maximum = 1023;
			this->humPot2->Name = L"humPot2";
			this->humPot2->Size = System::Drawing::Size(78, 45);
			this->humPot2->TabIndex = 5;
			this->humPot2->TickFrequency = 100;
			this->humPot2->Scroll += gcnew System::EventHandler(this, &Main_Interface::humPot2_Scroll);
			// 
			// humPot3
			// 
			this->humPot3->Location = System::Drawing::Point(10, 89);
			this->humPot3->Margin = System::Windows::Forms::Padding(2);
			this->humPot3->Maximum = 1023;
			this->humPot3->Name = L"humPot3";
			this->humPot3->Size = System::Drawing::Size(78, 45);
			this->humPot3->TabIndex = 6;
			this->humPot3->TickFrequency = 100;
			this->humPot3->Scroll += gcnew System::EventHandler(this, &Main_Interface::humPot3_Scroll);
			// 
			// humPot4
			// 
			this->humPot4->Location = System::Drawing::Point(110, 89);
			this->humPot4->Margin = System::Windows::Forms::Padding(2);
			this->humPot4->Maximum = 1023;
			this->humPot4->Name = L"humPot4";
			this->humPot4->Size = System::Drawing::Size(78, 45);
			this->humPot4->TabIndex = 6;
			this->humPot4->TickFrequency = 100;
			this->humPot4->Scroll += gcnew System::EventHandler(this, &Main_Interface::humPot4_Scroll);
			// 
			// Main_Interface
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(830, 526);
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->Actuadores);
			this->Controls->Add(this->pictureBox2);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->menuStrip1);
			this->Controls->Add(this->groupBox3);
			this->MainMenuStrip = this->menuStrip1;
			this->Margin = System::Windows::Forms::Padding(2);
			this->Name = L"Main_Interface";
			this->Text = L"Farmbot Simulator";
			this->groupBox1->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox2))->EndInit();
			this->Actuadores->ResumeLayout(false);
			this->Actuadores->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ActPump))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ActLed))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->ActValvle))->EndInit();
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->humPot1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->humPot2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->humPot3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->humPot4))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: void InitializeVariables() {
		this->x0 = this->lblXY->Location.X;
		this->y0 = this->lblXY->Location.Y;
		this->z0 = this->lblXZ->Location.Y;
		cout << "intial Position" << endl;
		cout << "X:" <<this->x0 << " Y:" << this->y0 << " Z:" << this->z0 << endl;

		this->x = this->x0;
		this->y = this->y0;
		this->z = this->z0;

		this->Valvle = false;
		this->Led = false;
		this->Pump = false;
	}
	private: void findPorts(void)
	{
		// get port names

		// add string array to combobox
		this->comboBoxCom->Items->AddRange(this->serialPort1->GetPortNames());

	}
	private: void update_position() {;
	
		if (this->x_act != this->x) {
			cout << "Changed X from" << this->x << "To" << this->x_act << endl;
			this->x = this->x_act;
			this->lblXY->Location = System::Drawing::Point(this->x, this->y);
			this->lblXZ->Location = System::Drawing::Point(this->x, this->z);
			cout << "New Location" << this->lblXY->Location.X << endl;
		}
	
		if (this->y_act != this->y) {
			cout << "Changed Y from" << this->y << "To" << this->y_act;
			this->y = this->y_act;
			this->lblXY->Location = System::Drawing::Point(this->x, this->y);
			cout << "New Location" << this->lblXY->Location.Y << endl;
		}
		if (this->z_act != this->z) {
			cout << "Changed Z from" << this->z << "To" << this->z_act;
			this->z = this->z_act;
			this->lblXZ->Location = System::Drawing::Point(this->x, this->z);
			cout << "New Location" << this->lblXZ->Location.X << endl;
		}
			

	}
	private: void update_actuators(bool p_pump,bool p_valve,bool p_led) {
		if (this->Pump == 0 && this->Pump != p_pump) {
			this->ActPump->Image = Image::FromFile("./images/pump_on.png");
			this->Pump = p_pump;
			cout << "Pump cambió a 1" << endl;
		}
		if (this->Pump == 1 && this->Pump != p_pump) {
			this->ActPump->Image = Image::FromFile("./images/pump_off.png");
			this->Pump = p_pump;
			cout << "Pump cambió a 0" << endl;
		}
		if (this->Valvle == 0 && this->Valvle != p_valve) {
			this->ActValvle->Image = Image::FromFile("./images/valvle_on.png");
			this->Valvle = p_valve;
			cout << "Valve cambió a 1" << endl;
		}
		if (this->Valvle == 1 && this->Valvle != p_valve) {
			this->ActValvle->Image = Image::FromFile("./images/valvle_off.png");
			this->Valvle = p_valve;
			cout << "Valve cambió a 0" << endl;
		}
		if (this->Led == 0 && this->Led != p_led) {
			this->ActLed->Image = Image::FromFile("./images/led_on.png");
			this->Led = p_led;
			cout << "Led cambió a 1" << endl;
		}
		if (this->Led == 1 && this->Led != p_led) {
			this->ActLed->Image = Image::FromFile("./images/led_off.png");
			this->Led = p_led;
			cout << "Led cambió a 0" << endl;
		}

	}
	static void thread1(System::Object^ obj) {
		Main_Interface^ interface = (Main_Interface^)obj;
		while (!interface->finishThread)
		{	
			//cout << "Entre al Thread del farmbot loop" << endl;
			interface->farmbot->loop();

			//Updates Actuators
			bool led_state = ArduinoPins::getInstance()->searchPin(LED_PIN).getValue();
			bool pump_state = ArduinoPins::getInstance()->searchPin(VACUUM_PIN).getValue();
			bool valve_state = ArduinoPins::getInstance()->searchPin(WATER_PIN).getValue();
			interface->update_actuators(pump_state, valve_state, led_state);
			
			//Updates Position
			interface->x_act = interface->x0 +CurrentState::getInstance()->getPosX()*360/3000;
			interface->y_act = interface->y0 + CurrentState::getInstance()->getPosY()*(388-14)/3000;
			interface->z_act = interface->z0 + CurrentState::getInstance()->getPosZ()*(45-15)/500;
			cout << "X:" << interface->x_act << " Y:" << interface->y_act << " Z:" << interface->z_act << endl;
			interface->update_position();
			Thread::Sleep(100);
			
		}

	}

	private: System::Void buttonInit_Click(System::Object^ sender, System::EventArgs^ e) {
		//this->textBox2->Text = String::Empty;
		if (this->comboBoxBaud->Text == String::Empty || this->comboBoxCom->Text == String::Empty)
		{
			MessageBox::Show("Debe Escoger un puerto COM y un BAUDRATE",
				"Error",
				MessageBoxButtons::OK,
				//MessageBoxIcon::Warning // for Warning  
				MessageBoxIcon::Error // for Error 
				//MessageBoxIcon.Information  // for Information
				//MessageBoxIcon.Question // for Question
			);
		}
		else {
			try {
				// make sure port isn't open	
				if (!this->serialPort1->IsOpen) {
					this->serialPort1->PortName = this->comboBoxCom->Text;
					this->serialPort1->BaudRate = Int32::Parse(this->comboBoxBaud->Text);
					//open serial port 
					this->serialPort1->Open();
					this->progressBar1->Value = 100;
					farmbot = gcnew Farmbot(this->serialPort1);
					newThread1 = gcnew Thread(gcnew ParameterizedThreadStart(&thread1));
					this->finishThread = false;
					newThread1->Start(this);

				}
				else {
					MessageBox::Show("Farmbot Already Running to change port press Close and select a new one",
						"Error",
						MessageBoxButtons::OK,
						MessageBoxIcon::Error // for Error 
					);
				}

			}

			catch (UnauthorizedAccessException^) {
				MessageBox::Show("COM port already open by another aplication",
					"Error",
					MessageBoxButtons::OK,
					MessageBoxIcon::Error // for Error 
				);
			}
		}
	}
	
	private: System::Void buttonClose_Click(System::Object^ sender, System::EventArgs^ e) {
		//Wait the other Thread to finish
		finishThread = true;
		this->newThread1->Join();
		//close serialPort
		this->serialPort1->Close();
		// update progress bar
		this->progressBar1->Value = 0;
		// Enable the init button
		this->buttonInit->Enabled = true;
		//destroys farmbot
		this->farmbot = nullptr;
		

	}
	private: System::Void buttonRead_Click(System::Object^ sender, System::EventArgs^ e) {
		// check if port is ready for reading
		if (this->serialPort1->IsOpen) {
			// Reset the text in the result label.


			// this will read manually
			try {
				this->commandHistory->AppendText("\r\nReceive->");
				this->commandHistory->AppendText(this->serialPort1->ReadLine());
				this->commandHistory->ScrollToCaret();
			}
			catch (TimeoutException^) {
				this->commandHistory->AppendText("\r\nReceive->");
				this->commandHistory->AppendText("Timeout Exception");
			}
		}
		else
			// give error warning
			this->commandHistory->AppendText("\r\nReceive->");
		this->commandHistory->AppendText("Port Not Opened");
	}
private: System::Void humPot1_Scroll(System::Object^ sender, System::EventArgs^ e) {
	if (farmbot != nullptr) {
		int humedad = this->humPot1->Value;
		//A0 es el pin 97
		ArduinoPins::getInstance()->digitalWrite(97, humedad);
	}
}
private: System::Void humPot2_Scroll(System::Object^ sender, System::EventArgs^ e) {
	if (farmbot != nullptr) {
		int humedad = this->humPot2->Value;
		//A1 es el pin 96
		ArduinoPins::getInstance()->digitalWrite(96, humedad);
	}
}
private: System::Void humPot3_Scroll(System::Object^ sender, System::EventArgs^ e) {
	if (farmbot != nullptr) {
		int humedad = this->humPot3->Value;
		//A2 es el pin 95
		ArduinoPins::getInstance()->digitalWrite(95, humedad);
	}
}
private: System::Void humPot4_Scroll(System::Object^ sender, System::EventArgs^ e) {
	if (farmbot != nullptr) {
		int humedad = this->humPot4->Value;
		//A3 es el pin 94
		ArduinoPins::getInstance()->digitalWrite(94, humedad);
	}
}

};
}
