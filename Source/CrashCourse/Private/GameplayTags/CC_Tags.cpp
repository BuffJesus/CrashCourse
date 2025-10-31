#include "GameplayTags/CC_Tags.h"

namespace CCTags
{
	namespace CCAbilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateOnGiven, "CCTags.CCAbilities.ActivateOnGiven", "Tag for the Ability to activate on given");
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "CCTags.CCAbilities.Primary", "Tag for the Primary Ability");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "CCTags.CCAbilities.Secondary", "Tag for the Secondary Ability");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tertiary, "CCTags.CCAbilities.Tertiary", "Tag for the Tertiary Ability");
	}

	namespace Events
	{
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "CCTags.Events.Enemy.HitReact", "Tag for the Enemy HitReact Event");
		}
	}	
}