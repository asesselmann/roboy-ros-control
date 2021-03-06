#include "CylindricalWrapping.hpp"

using namespace roboy_simulation;

CylindricalWrapping::CylindricalWrapping(): IViaPoints(math::Vector3(0,0,0), Type::CYLINDRICAL), stateMachine(StateMachine()),
                                    radius(0), prevCoord(math::Vector3(0,0,0)), nextCoord(math::Vector3(0,0,0)),
                                    prevCoordPlane(math::Vector3(0,0,0)), nextCoordPlane(math::Vector3(0,0,0)),
                                    prevForcePointPlane(math::Vector3(0,0,0)), nextForcePointPlane(math::Vector3(0,0,0)),
                                    normal(math::Vector3(0,0,0)), arcAngle(0)
{

};

CylindricalWrapping::CylindricalWrapping(math::Vector3 point): CylindricalWrapping()
{
    localCoordinates = point;
};

CylindricalWrapping::CylindricalWrapping(math::Vector3 point, double radius, int state, int counter): CylindricalWrapping(point)
{
    this->radius = radius;
    this->stateMachine.state = (StateMachine::State) state;
    this->stateMachine.revCounter = counter;
};

void CylindricalWrapping::UpdateForcePoints()
{
    prevCoord = prevPoint->nextForcePoint;
    nextCoord = nextPoint->prevForcePoint;

    //calculate normal onto plane
    math::Vector3 unit_normal = linkRotation.operator*(math::Vector3 (0,0,1));
    //project insertion and fixation point onto xy plane of the cylinder
    prevCoordPlane = prevCoord - ((prevCoord-globalCoordinates).Dot(unit_normal))*unit_normal;
    nextCoordPlane = nextCoord - ((nextCoord-globalCoordinates).Dot(unit_normal))*unit_normal;

    stateMachine.UpdateState(prevCoordPlane, nextCoordPlane, globalCoordinates, radius);

    //if muscle is not wrapping, use straight line calculation
    if (stateMachine.state == StateMachine::NOTWRAPPING)
    {
        prevForcePoint = globalCoordinates;
        nextForcePoint = globalCoordinates;
        previousSegmentLength = (prevCoord - prevForcePoint).GetLength();
    }

    //compute tangent points
    //compute unit vectors and according length
    double l_j1 = (prevCoordPlane-this->globalCoordinates).GetLength();
    math::Vector3 j1 = (prevCoordPlane-this->globalCoordinates)/l_j1;
    double l_j2 = (nextCoordPlane-this->globalCoordinates).GetLength();
    math::Vector3 j2 = (nextCoordPlane-this->globalCoordinates)/l_j2;

    //compute normal,
    normal = j1.Cross(j2);

    //compute k1, k2
    math::Vector3 k1 = j1.Cross(normal);
    math::Vector3 k2 = normal.Cross(j2);

    //compute length of a1, a2, b1, b2
    double a1 = radius*radius/l_j1;
    double a2 = radius*radius/l_j2;
    double b1 = sqrt(radius*radius - a1*a1);
    double b2 = sqrt(radius*radius - a2*a2);

    if (stateMachine.state == StateMachine::POSITIVE)
    {
        this->prevForcePointPlane = this->globalCoordinates + a1*j1 - b1*k1;
        this->nextForcePointPlane = this->globalCoordinates + a2*j2 - b2*k2;
    }
    else if (stateMachine.state == StateMachine::NEGATIVE)
    {
        this->prevForcePointPlane = this->globalCoordinates + a1*j1 + b1*k1;
        this->nextForcePointPlane = this->globalCoordinates + a2*j2 + b2*k2;
    }

    //update revolution counter
    double projection = (prevCoordPlane - this->prevForcePointPlane).Dot(this->nextForcePointPlane - this->prevForcePointPlane);
    stateMachine.UpdateRevCounter(projection);

    //calculate the wrapping angle
    double angle = acos(1 - (this->prevForcePointPlane-this->nextForcePointPlane).GetLength()/2*radius*radius);
    arcAngle = 2*(boost::math::constants::pi<double>())*ceil(stateMachine.revCounter/2);
    arcAngle += (stateMachine.revCounter % 2 == 0)?(angle):(-angle);

    double l_insertion = (prevCoordPlane - prevForcePointPlane).GetLength();
    double l_fixation = (nextCoordPlane - nextForcePointPlane).GetLength();
    double l_arc = arcAngle*radius;
    double insertionDistance = (prevCoord - prevCoordPlane).GetLength();
    double fixationDistance = (nextCoord - nextCoordPlane).GetLength();

    //calculate tangent point distance to the plane
    double iTDistance = insertionDistance + (l_insertion)*(fixationDistance - insertionDistance)/(l_insertion + l_arc + l_fixation);
    //project tangent point into R3
    prevForcePoint = prevForcePointPlane + iTDistance*unit_normal;

    //calculate tangent point distance to the plane
    double fTDistance = fixationDistance + (l_fixation)*(insertionDistance - fixationDistance)/(l_fixation + l_arc + l_insertion);
    //project tangent point into R3
    nextForcePoint = nextForcePointPlane + fTDistance*unit_normal;

    //calculate the lines of action and the muscle's length
    double distance = iTDistance - fTDistance;

    //calculate the lines of action and the muscle's length
    previousSegmentLength = (prevCoord - this->prevForcePoint).GetLength() + sqrt(distance*distance + l_arc*l_arc) + (this->nextForcePoint - nextCoord).GetLength();
};
