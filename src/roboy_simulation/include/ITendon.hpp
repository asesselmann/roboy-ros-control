#ifndef _GAZEBO_ITENDON_HPP_
#define _GAZEBO_ITENDON_HPP_

#include "IActuator.hpp"
#include "IViaPoints.hpp"
#include "SphericalWrapping.hpp"
#include "CylindricalWrapping.hpp"
#include "MeshWrapping.hpp"


#include <vector>
#include <boost/numeric/odeint.hpp>

namespace roboy_simulation
{
    using namespace std;
	using namespace boost::numeric::odeint;
	using namespace gazebo;
/*
    struct tendonType {
		vector<math::Vector3> MidPoint;
		vector<math::Vector3> Vector;
		//might need it to calculate length
		vector<math::Vector3> Orientation;
		vector<double> Pitch;
		vector<double> Roll;
	};
*/
	struct SEE {
		double stiffness = 1.0;
		double length = 0.1;
		double expansion = 0.0;
		double force = 0.0;
		double length0 = 0.1;
	};

    class ITendon {
        public:

        SEE see;

        ITendon();

		////////////////////////////////////////
		/// \brief Calculate torque for an electric motor model.
		/// \param[in] _current Input electric current
		/// \param[in] _torqueConstant Motor's torque constant
		/// \param[in] _spindleRadius Radius of the spindle that coils up the tendon
		/// \return Calculated force according to the model
		double ElectricMotorModel(const double _current, const double _torqueConstant,
								  const double _spindleRadius);

		////////////////////////////////////////
		/// \brief Calculate elastic force of the series elastic element
		/// \param[in] _length0 Resting length of the SEE
		/// \param[in] _length Current length of the SEE
		/// \param[in] _stiffness Deafault values for stiffness of the SEE
		/// \return Elastic force in N
		double ElasticElementModel(const double _length0, const double _length,
								   double _stiffness, const double _speed,
								   const double _spindleRadius, const double _time);

        ////////////////////////////////////////
	    /// \brief Calculate elastic force of the series elastic element
	    /// \param[in] see the series elastic element
	    /// \param[in] _length Current length of the spring
	    void ElasticElementModel(SEE &see, const double &length);

		//static void GetTendonInfo(vector<math::Vector3> &viaPointPos, tendonType *tendon_p);

        double muscleLength;
        double tendonLength;
        double initialTendonLength;
        bool firstUpdate;

	private:
		////////////////////////////////////////
		/// \brief Calculate the dot product between two vectors
		/// \param[in] _v1 vector 1 coordinates
		/// \param[in] _v2 vector 2 coordinates
		/// \return Dot product
		double DotProduct(const math::Vector3 &_v1, const math::Vector3 &_v2);

		////////////////////////////////////////
		/// \brief Calculate the angle between two vectors
		/// \param[in] _v1 vector 1 coordinates
		/// \param[in] _v2 vector 2 coordinates
		/// \return Angle between two vectors in radians
		double Angle(const math::Vector3 &_v1, const math::Vector3 &_v2);
	};
}

#endif
