#include "ITendon.hpp"

using namespace roboy_simulation;

    ITendon::ITendon(): muscleLength(0), tendonLength(0), initialTendonLength(0), firstUpdate(true)
    {

    };

	double ITendon::ElectricMotorModel(const double _current, const double _torqueConstant,
									   const double _spindleRadius) {
		double motorForce;

		if (_current >= 0) {
			motorForce = _current * _torqueConstant / _spindleRadius;
		}
		else {
			motorForce = 0;
		}

		return motorForce;
	}


	double ITendon::ElasticElementModel(const double _length0, const double _length, double _stiffness,
										const double _speed, const double _spindleRadius, const double _time) {
		// double realTimeUpdateRate=1000;
		double windingLength = _spindleRadius * _speed * _time;
		double displacement;
		displacement = windingLength + _length - _length0;

		// gzdbg << "displacement: "
		// 	  << displacement
		// 	  << "\n"
		//          << "windingLength: "
		// 	  << windingLength
		// 	  << "\n";

		double elasticForce;

		if (displacement >= 0) {
			elasticForce = displacement * _stiffness;
		}
		else {
			elasticForce = 0;
		}

		//return _stiffness[0] + (displacement*_stiffness[1]) + (displacement*displacement*_stiffness[2]) +
		//			(displacement*displacement*displacement*_stiffness[3]) ;
		//return displacement*_stiffness[0];
		return elasticForce;

	}

	void ITendon::ElasticElementModel(SEE &see, const double &length)
    {
        see.expansion = length - see.length0;

        if (see.expansion>=0)
        {
            see.force=see.expansion*see.stiffness;
        }
        else
        {
            see.force=0;
        }
    }

/*
	math::Vector3 ITendon::CalculateForce(double _elasticForce, double _motorForce,
										  const math::Vector3 &_tendonOrien) {
		// math::Vector3 diff = _fixationP - _instertionP;

		//    double tendonForce;

		//if (_elasticForce+_motorForce>=0)
		//{
		//	tendonForce=_elasticForce+_motorForce;
		//}
		//else
		//{
		//	tendonForce=0;
		//}

		return _tendonOrien * (_elasticForce + _motorForce);

	}
*/
/*
	void ITendon::GetTendonInfo(vector<math::Vector3> &viaPointPos, tendonType *tendon_p)//try later with pointer
	{
		for (int i = 0; i < viaPointPos.size() - 1; i++) {
			tendon_p->MidPoint.push_back((viaPointPos[i] + viaPointPos[i + 1]) / 2);
			tendon_p->Vector.push_back(viaPointPos[i] - viaPointPos[i + 1]);
			tendon_p->Orientation.push_back(tendon_p->Vector[i] / tendon_p->Vector[i].GetLength());
			tendon_p->Pitch.push_back(atan(tendon_p->Orientation[i][0] / tendon_p->Orientation[i][2]));
			tendon_p->Roll.push_back(
					-acos(sqrt((pow(tendon_p->Orientation[i][0], 2) + pow(tendon_p->Orientation[i][2], 2)))));
		}
	}
*/

	double ITendon::DotProduct(const math::Vector3 &_v1, const math::Vector3 &_v2) {
		return _v1.x * _v2.x + _v1.y * _v2.y + _v1.z * _v2.z;
	}


	double ITendon::Angle(const math::Vector3 &_v1, const math::Vector3 &_v2) {
		return acos(_v1.Dot(_v2) / _v1.GetLength() * _v2.GetLength());
	}
