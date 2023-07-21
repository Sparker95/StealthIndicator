/*
Shows values from SCR_AIRecognitionProbeComponent.
Also has basic filter built-in to smooth the visualized value.
*/

class SCR_StealthIndicatorInfoDisplay : SCR_InfoDisplay
{
	protected ProgressBarWidget m_ProgressBar;
	protected SCR_AIRecognitionProbeComponent m_RecognitionComp;
	
	[Attribute()]
	protected ref Color m_ColorLow;
	[Attribute()]
	protected ref Color m_ColorMedium;
	[Attribute()]
	protected ref Color m_ColorHigh;
	
	protected float m_fPrevFilteredValue;
	
	override void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);
		
		m_ProgressBar = ProgressBarWidget.Cast(GetRootWidget().FindAnyWidget("m_ProgressBar"));
		
		PlayerController pc = PlayerController.Cast(owner);
		m_RecognitionComp = SCR_AIRecognitionProbeComponent.Cast(pc.FindComponent(SCR_AIRecognitionProbeComponent));
		
		// Set up progress bar limits
		m_ProgressBar.SetMin(0);
		m_ProgressBar.SetMax(2.0); // !!! Note that max is 2.0
		
		Show(true);
	}
		
	override void UpdateValues(IEntity owner, float timeSlice)
	{
		super.UpdateValues(owner, timeSlice);
		
		if (!m_RecognitionComp)
			return;
		
		float recDetect, recIdent;
		m_RecognitionComp.GetRecognitionMax(recDetect, recIdent);
		
		float totalValue = recDetect + recIdent;
		float valueFiltered = Filter(totalValue);
		
		if (totalValue == 0)
		{
			// Hide everything
			Show(false, UIConstants.FADE_RATE_DEFAULT);
		}
		else
		{
			Show(true, UIConstants.FADE_RATE_DEFAULT);
			
			m_ProgressBar.SetCurrent(valueFiltered);
			
			// Change color, but depending on current value, not filtered
			Color color;
			if (totalValue < 1.0)
				color = m_ColorLow;
			else if (totalValue < 2.0)
				color = m_ColorMedium;
			else
				color = m_ColorHigh;
			
			m_ProgressBar.SetColor(color);
		}
	}
	
	float Filter(float inValue)
	{
		float outValue = 0.05*inValue + 0.95*m_fPrevFilteredValue;
		m_fPrevFilteredValue = outValue;
		
		return outValue;
	}
}