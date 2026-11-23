#include "AIConstructor_RBS.h"
#include "AIActor_Driver.h"


bool AIConstructor_RBS::Init() {

	method = AIConstructorBase::Method::RBS;
	return true;
}


/*
DefineActions creates all the possible actions that the AI can perform
*/
void AIConstructor_RBS::DefineActions()
{

	//Define the function for  turning away from collision
	auto avoidFunction = [](AIBrainBlackboardBase& bb) -> ActionStatus {
		AIActor_Driver* actor = static_cast<AIActor_Driver*>(bb.GetActorContext());
		float curXpos = bb.GetValue<float>("TrackXPos");

		// always turn towards center
		/*
		if (curXpos > 0)
		{
			actor->Turn(0.4f);
			if (curXpos > 0.2)
			{
				actor->Turn(-1);
			}
		}
		else
		{
			actor->Turn(0.4f);
			if (curXpos < -0.2f)
			{
				actor->Turn(1);
			}
		}
		*/

		float turn = -curXpos;
		if (std::abs(turn) < 0.4f)
		{
			turn *= 10;
		}
		if (turn < -1)
		{
			turn = -1;
		}
		else if (turn > 1)
		{
			turn = 1;
		}
		actor->Turn(turn);

		//Pseudo for probably better
		//get position of obstacle
		//invert its position
		//set to 1
		//turn

		// NOTE: If you are using an AI Method where actions continue beween updates (e.g. Behaviour Trees)
		//   actions should return ACTION_RUNNING, ACTION_SUCCESS or ACTION_FAILURE as appropriate
		//   to ensure that the same action will repeatedly be called each update tick
		return ActionStatus::ACTION_SUCCESS;
		};


	// Link the function to an identifier
	AddActionByName("ActionAvoid", avoidFunction);




	//Define the function accelerating and centering
	auto accelFunction = [](AIBrainBlackboardBase& bb) -> ActionStatus {
		AIActor_Driver* actor = static_cast<AIActor_Driver*>(bb.GetActorContext());

		float curXpos = bb.GetValue<float>("TrackXPos");
		bool flag = true; //used to mark if the car is on the edge - on turns, if the car is too fast it may be unable to escape the edge. That is bad.

		if (curXpos > 0)
		{
			actor->Turn(0.1f);
			if (curXpos > 0.15)
			{
				actor->Turn(-0.15f);
				if (curXpos > 0.5)
				{
					actor->Turn(-0.2f);
					if (curXpos > 0.8)
					{
						actor->Turn(-0.25f);
						if (curXpos > 0.9)
						{
							actor->Turn(-0.4f);
							//flag = false;
						}
					}
				}
			}
		}
		else if (curXpos < 0)
		{
			actor->Turn(-0.1f);
			if (curXpos < -0.15)
			{
				actor->Turn(0.15f);
				if (curXpos < -0.5)
				{
					actor->Turn(0.2f);
					if (curXpos < -0.8)
					{
						actor->Turn(0.25f);
						if (curXpos < -0.9)
						{
							actor->Turn(0.4f);
							//flag = false;
						}
					}
				}
			}
		}

		if (flag)
		{
			actor->Accel(1.f);
		}
		else
		{
			actor->Decel(1.f);
		}


		// NOTE: If you are using an AI Method where actions continue beween updates (e.g. Behaviour Trees)
		//   actions should return ACTION_RUNNING, ACTION_SUCCESS or ACTION_FAILURE as appropriate
		//   to ensure that the same action will repeatedly be called each update tick
		return ActionStatus::ACTION_SUCCESS;
		};


	// Link the function to an identifier
	AddActionByName("ActionAccel", accelFunction);
	

}


/*
	DefineConsiderations creates all Considerations that will be linked to Options
// Options will only be selected if the Condition is true
*/
void AIConstructor_RBS::DefineConsiderations()
{


	// is there an obstacle of any type in reactDistance
	auto cObstacle = [](AIBrainBlackboardBase& bb) -> bool {

		int reactDistance = 10;

		// get all collision types from blackboard
		std::vector< AIMath_Global::Vector2f > _oil = bb.GetValue<	std::vector< AIMath_Global::Vector2f > >("ObstacleOil");
		std::vector< AIMath_Global::Vector2f > _bollard = bb.GetValue<	std::vector< AIMath_Global::Vector2f > >("ObstacleBollard");
		std::vector< AIMath_Global::Vector2f > _trash = bb.GetValue<	std::vector< AIMath_Global::Vector2f > >("ObstacleTrash");

		// check if in car's path
		bool obstacleInPath = false;
		float curX = bb.GetValue<float>("TrackXPos");
		float _collideDist = 0.2f;

		for (auto c : _oil)
		{
			if (std::abs(c.x - curX) < _collideDist && c.y <= reactDistance)
			{
				//std::cout << " Oil detected at " << c.y << std::endl;
				return true;
			}
		}
	
		for (auto c : _bollard)
		{
			if (std::abs(c.x - curX) < _collideDist && c.y <= reactDistance)
			{
				//std::cout << " Bollard detected at " << c.y << std::endl;
				return true;
			}
		}

		for (auto c : _trash)
		{
			if (std::abs(c.x - curX) < _collideDist && c.y <= reactDistance)
			{
				//std::cout << " Trash detected at " << c.y << std::endl;
				return true;
			}
		}
		return(false);

		};



	AddConsiderationByName("ConsiderationObstacle", cObstacle);


	// is there NOT an obstacle of any type in reactDistance

	auto cNotObstacle = [](AIBrainBlackboardBase& bb) -> bool {

		int reactDistance = 10;

		// get all collision types from blackboard
		std::vector< AIMath_Global::Vector2f > _oil = bb.GetValue<	std::vector< AIMath_Global::Vector2f > >("ObstacleOil");
		std::vector< AIMath_Global::Vector2f > _bollard = bb.GetValue<	std::vector< AIMath_Global::Vector2f > >("ObstacleBollard");
		std::vector< AIMath_Global::Vector2f > _trash = bb.GetValue<	std::vector< AIMath_Global::Vector2f > >("ObstacleTrash");

		// check if in car's path
		bool obstacleInPath = false;
		float curX = bb.GetValue<float>("TrackXPos");
		float _collideDist = 0.1f;
		
		for (auto c : _oil)
		{
			if (abs(c.x - curX) < _collideDist && c.y <= reactDistance)
			{
				return false;
			}
		}

		for (auto c : _bollard)
		{
			if (abs(c.x - curX) < _collideDist && c.y <= reactDistance)
			{
				return false;
			}
		}

		for (auto c : _trash)
		{
			if (abs(c.x - curX) < _collideDist && c.y <= reactDistance)
			{
				return false;
			}
		}
		return(true);

		};



	AddConsiderationByName("ConsiderationNOTObstacle", cNotObstacle);


}



/*
* DefineOptions links the Options with Considerations and Actions
*/
void AIConstructor_RBS::DefineOptions()
{

	AddOptionByName("OptionAvoid", "ActionAvoid");
		AddOptionConsideration("OptionAvoid", "ConsiderationObstacle");
	AddOptionByName("OptionAccel", "ActionAccel");
		AddOptionConsideration("OptionAccel", "ConsiderationNOTObstacle");
}



