#include "GameRunStage.h"

#include "DebugEngine/DebugNew.h"

/**
 *
 */
#ifdef SHOW_DEBUG
void AfficheBufferButton(FontDX& font_, BufferButton& button_);
#endif

/**
 *
 */
GameRunStage::GameRunStage()
{
}

/**
 *
 */
GameRunStage::~GameRunStage()
{
}

/**
 *
 */
bool GameRunStage::Init()
{
	sStageLoadingParams stageParam;

	m_pGraphic = &GameCore::Instance().GetGraphic();
	m_pPad1 = &GameCore::Instance().GetPad(0);

	m_pGraphic->SetAmbientLight(100, 100, 100);

	m_pGraphic->EnableZBuffer(true);
	m_pGraphic->EnableLighting(true);

	m_Light.SetAmbientColor(100, 100, 100);
	m_Light.SetDiffuseColor(200, 200, 200);
	m_Light.SetSpecularColor(100, 100, 100);
	m_Light.SetFalloff(0.1f);
	m_Light.SetType(D3DLIGHT_DIRECTIONAL);
	m_Light.SetRange(1000.0f);
	m_Light.SetPhi(0.1f);
	m_Light.Point(-100.0f, 100.0f, 100.0f, 0.0f, 0.0f, 0.0f);

	m_Light2.SetAmbientColor(100, 100, 100);
	m_Light2.SetDiffuseColor(200, 200, 200);
	m_Light2.SetSpecularColor(100, 100, 100);
	m_Light2.SetFalloff(0.1f);
	m_Light2.SetType(D3DLIGHT_DIRECTIONAL);
	m_Light2.SetRange(1000.0f);
	m_Light2.SetPhi(0.1f);
	m_Light2.Point(-100.0f, 100.0f, -100.0f, 0.0f, 0.0f, 0.0f);

	m_pGraphic->SetLight(0, &m_Light);
	m_pGraphic->EnableLight(0);
	m_pGraphic->SetLight(1, &m_Light2);
	m_pGraphic->EnableLight(1);

	m_Font.Create("Courrier New", 10);

	stageParam.characP1FileName.append("ryu.xml");
	stageParam.characP1Type = CHARACTER_CONTROLLER_TYPE_DIRECT;
	stageParam.characP2FileName.append("ryu.xml");
	stageParam.characP2Type = CHARACTER_CONTROLLER_IA;
	stageParam.stageFileName.append("sol.X");

	if (m_Stage.Load(stageParam) == false)
	{
		return false;
	}

	GameCore::Instance().GetGraphic().StartTime();

	return true;
}

/**
 *
 */
bool GameRunStage::Update(float elapsedTime_)
{
	m_Stage.Update(elapsedTime_);

	GameCore::Instance().ReadAllInputDevice();

	m_Stage.UpdateBufferButton(m_pPad1, nullptr);
	//m_BufferButtonP1.Update( m_pPad1 );

	if (m_pPad1->ButtonPressed(PAD_BUTTON_8))
	{
		SetDestroy(true);
		return false;
	}

	if (m_pPad1->ButtonHeld(PAD_FIRE)) // bouton pour menu
	{

	}

	return true;
}

/**
 *
 */
void GameRunStage::Draw(float elapsedTime_)
{
	static char buf[512];

	m_pGraphic->Clear(0xffb4b4b4);

	m_Stage.Render();

	sprintf_s(buf, 512, "FPS : %d", m_pGraphic->GetFPS());
	m_Font.Print(buf, 0, 0);

#ifdef SHOW_DEBUG
	m_Stage.ShowDebug(m_Font);
#endif

#ifdef _DEBUG
	/*
	sprintf_s(buf, 512, "State : %.5d - Animation : %d (%f - %f)",
		(int) charac1.GetParameterValue( std::string("stateno") ),
		charac1.GetMesh()->GetCurrentAnimationSet(),
		charac1.GetMesh()->GetCurrentAnimationTime(),
		charac1.GetMesh()->GetCurrentAnimationTimeMax() );
	font.Print(buf, 0, 10);

	sprintf_s(buf, 512, "Controle : %d", (int) charac1.GetParameterValue( std::string("ctrl")) );
	font.Print(buf, 0, 20);*/
#endif // _DEBUG

#ifdef SHOW_DEBUG
	AfficheBufferButton(m_Font, m_Stage.GetBufferButtonP1());
#endif
}

/**
 *
 */
void AfficheBufferButton(FontDX& font_, BufferButton& button_)
{
	char buf[512];

	int x = 0;

	for (std::deque<BufferButton::sBufferButton>::iterator it = button_.GetBuffer().begin(); it != button_.GetBuffer().end(); ++it)
	{
		sprintf_s(buf, "%d", it->buttonPressed);
		font_.Print(buf, x, 550);

		sprintf_s(buf, "%d", it->buttonHeld);
		font_.Print(buf, x, 560);

		sprintf_s(buf, "%d", it->buttonReleased);
		font_.Print(buf, x, 570);

		sprintf_s(buf, "%d", it->time);
		font_.Print(buf, x, 580);

		x += 60;
	}
}
