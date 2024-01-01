/*
Measures how much perception objects have recognized our controlled entity.
This component needs to be attached to player controller.
*/

[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
class SCR_AIRecognitionProbeComponentClass : ScriptComponentClass
{
}

class SCR_AIRecognitionProbeComponent : ScriptComponent
{
	protected float m_fRecIdentMax;
	protected float m_fRecDetectMax;
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		PlayerController pc = PlayerController.Cast(owner);
		
		SCR_ChimeraCharacter myCharacter = SCR_ChimeraCharacter.Cast(pc.GetControlledEntity());
		if (!myCharacter)
		{
			Reset();
			return;
		}
		
		Faction myFaction = myCharacter.m_pFactionComponent.GetAffiliatedFaction();
		if (!myFaction)
		{
			Reset();
			return;
		}
		
		// If we're in vehicle, set teh vehicle to myEntity
		IEntity myCharacterOrVehicle;
		BaseCompartmentSlot compartmentSlot = myCharacter.GetCompartmentAccessComponent().GetCompartment();
		if (compartmentSlot)
			myCharacterOrVehicle = compartmentSlot.GetVehicle();
		else
			myCharacterOrVehicle = myCharacter;
		
		float recDetect, recIdent;
		float recDetectMax = 0;
		float recIdentMax = 0;
		
		array<SCR_ChimeraCharacter> allCharacters = SCR_ChimeraCharacter.GetAllCharacters();
		foreach (SCR_ChimeraCharacter character : allCharacters)
		{
			// Ignore if same faction
			if (!character.m_pFactionComponent || character.m_pFactionComponent.GetAffiliatedFaction() == myFaction)
				continue;
			
			// Ignore if destroyed
			if (character.m_DamageManager && character.m_DamageManager.IsDestroyed())
				continue;
			
			PerceptionComponent perc = character.m_PerceptionComp;
			
			BaseTarget tgt = perc.GetTargetPerceptionObject(myCharacterOrVehicle, ETargetCategory.UNKNOWN);
			if (!tgt)
				tgt = perc.GetTargetPerceptionObject(myCharacterOrVehicle, ETargetCategory.DETECTED);
			if (!tgt)
				tgt = perc.GetTargetPerceptionObject(myCharacterOrVehicle, ETargetCategory.ENEMY);
			
			// This character doesn't know about us
			if (!tgt)
				continue;
			
			tgt.GetAccumulatedRecognition(recDetect, recIdent);
			
			ETargetCategory category = tgt.GetTargetCategory();
			if (category == ETargetCategory.DETECTED)
			{
				// If we are in detected category, force recDetect to 1.0,
				// since hearing sensor puts us into detected category and doesn't affect recDetect.
				recDetect = 1.0;
			}
			else if (category == ETargetCategory.ENEMY)
			{
				recDetect = 1.0;
				recIdent = 1.0;
			}
			
			if (recDetect > recDetectMax)
				recDetectMax = recDetect;
			if (recIdent > recIdentMax)
				recIdentMax = recIdent;
		}
		
		m_fRecIdentMax = recIdentMax;
		m_fRecDetectMax = recDetectMax;
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnDiag(IEntity owner, float timeSlice)
	{
		DbgUI.Begin("Stealth Indicator Debug", 0, 0);
		
		DbgUI.Text(string.Format("Detect: %1", m_fRecDetectMax.ToString(4, 2)));
		DbgUI.Text(string.Format("Ident.: %1", m_fRecIdentMax.ToString(4, 2)));
		
		DbgUI.End();
	}
	
	//------------------------------------------------------------------------------------------------
	void Reset()
	{
		m_fRecIdentMax = 0;
		m_fRecDetectMax = 0;
	}
	
	//------------------------------------------------------------------------------------------------
	void GetRecognitionMax(out float detection, out float identification)
	{
		detection = m_fRecDetectMax;
		identification = m_fRecIdentMax;
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
	}
}
