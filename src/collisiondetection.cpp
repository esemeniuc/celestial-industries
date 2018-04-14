#include "collisiondetection.hpp"

#include <algorithm>
#include <array>
#include <map>

namespace CollisionDetection {
	CollisionInfo movingBodyCollidesWithStatic(BoundingBox staticBox, BoundingBox movingBox, glm::vec3 staticPosition, glm::vec3 movingStartPosition, glm::vec3 movingEndPosition, float totalTime)
    {
        return aabbMinkowskiCollisions(staticBox, movingBox, staticPosition, staticPosition, movingStartPosition, movingEndPosition, totalTime);
    }

    CollisionInfo aabbMinkowskiCollisions(BoundingBox a, BoundingBox b, glm::vec3 aStart, glm::vec3 aEnd, glm::vec3 bStart, glm::vec3 bEnd, float totalTime)
    {
        // Based off https://gamedev.stackexchange.com/questions/93035/whats-the-fastest-way-checking-if-two-moving-aabbs-intersect#93096
        // TODO: are their coordinates relative or not? this may affect results, but at least wont be subtle
        a = normalizeBoundingBox(a);
        b = normalizeBoundingBox(b);
        BoundingBox difference = minkowskiDiff(a, b);

        BoundingBox rotatedA = rotateBoundingBoxAboutOrigin(a);
        //BoundingBox sum = minkowskiSum(rotatedA, b);

        glm::vec3 aVelocity = (aEnd - aStart);
        glm::vec3 bVelocity = (bEnd - bStart);

        glm::vec3 relativeVelocity = aVelocity - bVelocity;
		glm::vec3 relativeDistance = relativeVelocity * totalTime;
        //// using velocity as a vertex to then check it against the minkowski sum to see if there is a collision
        LineSegment segment = originToVertex(relativeDistance);

        return segmentAABBCollision(segment, difference, totalTime);
    }

    CollisionInfo aabbMinkowskiCollisions(MovingBoundingBox a, MovingBoundingBox b, float totalTime)
    {
        return aabbMinkowskiCollisions(
            a.box, b.box,
            a.position, a.position + a.velocity*totalTime,
            b.position, b.position + b.velocity*totalTime, totalTime);
    }

    bool aabbsOverlap(BoundingBox a, BoundingBox b)
    {
        float abx = b.lowerCorner.x - a.upperCorner.x;
        float bax = a.lowerCorner.x - b.upperCorner.x;
        float aby = b.lowerCorner.z - a.upperCorner.z;
        float bay = a.lowerCorner.z - b.upperCorner.z;

        if (abx >= 0.0f || aby >= 0.0f)
            return false;

        if (bax >= 0.0f || bay >= 0.0f)
            return false;

        return true;
    }

	// A static collision check
	bool aabbsOverlap(MovingBoundingBox a, MovingBoundingBox b) {
		BoundingBox aBox = a.box;
		aBox.lowerCorner += a.position;
		aBox.upperCorner += a.position;
		BoundingBox bBox = b.box;
		bBox.lowerCorner += b.position;
		bBox.upperCorner += b.position;
		return aabbsOverlap(aBox, bBox);
	}

    BoundingBox normalizeBoundingBox(BoundingBox box)
    {
        BoundingBox result;
        result.upperCorner.x = std::max(box.upperCorner.x, box.lowerCorner.x);
        result.upperCorner.z = std::max(box.upperCorner.z, box.lowerCorner.z);
        result.lowerCorner.x = std::min(box.upperCorner.x, box.lowerCorner.x);
        result.lowerCorner.z = std::min(box.upperCorner.z, box.lowerCorner.z);
        return result;
    }

    BoundingBox rotateBoundingBoxAboutOrigin(BoundingBox box)
    {
        BoundingBox result;
        result.upperCorner.x = -box.upperCorner.x;
        result.upperCorner.z = -box.upperCorner.z;
        result.lowerCorner.x = -box.lowerCorner.x;
        result.lowerCorner.z = -box.lowerCorner.z;
        return normalizeBoundingBox(result);
    }

    BoundingBox minkowskiDiff(BoundingBox a, BoundingBox b) {
        BoundingBox result;
        result.lowerCorner.x = a.lowerCorner.x - b.upperCorner.x;
        result.upperCorner.z = a.upperCorner.z - b.lowerCorner.z;
        float aWidth = a.upperCorner.x - a.lowerCorner.x;
        float bWidth = b.upperCorner.x - b.lowerCorner.x;
        result.upperCorner.x = result.lowerCorner.x + aWidth + bWidth;
        float aHeight = a.upperCorner.z - a.lowerCorner.z;
        float bHeight = b.upperCorner.z - b.lowerCorner.z;
        result.lowerCorner.z = result.upperCorner.z - aHeight - bHeight;
        return result;
    }

    BoundingBox minkowskiSum(BoundingBox a, BoundingBox b)
    {
        BoundingBox result;
        result.lowerCorner.x = a.lowerCorner.x + b.lowerCorner.x;
        result.lowerCorner.z = a.lowerCorner.z + b.lowerCorner.z;
        result.upperCorner.x = a.upperCorner.x + b.upperCorner.x;
        result.upperCorner.z = a.upperCorner.z + b.upperCorner.z;
        return result;
    }


    LineSegment originToVertex(glm::vec3 vertex)
    {
        // This is some rocket-science-heart-surgery level of complicated
        return {
            { 0,0,0 },
            vertex
        };
    }

    IntersectionCollisionInfo segmentsCollision1(LineSegment a, LineSegment b) {

        // Lifted verbatim from http://alienryderflex.com/intersect/
        float Ax = a.start.x;
        float Ay = a.start.z; // using p0_y and not z so i can direclty compare to the SO code
        float Bx = a.end.x;
        float By = a.end.y;
        float Cx = b.start.x;
        float Cy = b.start.z;
        float Dx = b.end.x;
        float Dy = b.end.z;

        float distAB, theCos, theSin, newX, ABpos;

        IntersectionCollisionInfo NO = {
            false, 0.0f,{ 0.0f,0.0f,0.0f }
        };

        //  Fail if either line segment is zero-length.
        if ((Ax == Bx && Ay == By) || (Cx == Dx && Cy == Dy)) return NO;

        //  Fail if the segments share an end-point.
        if ((Ax == Cx && Ay == Cy) || (Bx == Cx && By == Cy) ||
            (Ax == Dx && Ay == Dy) || (Bx == Dx && By == Dy)) {
            return NO;
        }

        //  (1) Translate the system so that point A is on the origin.
        Bx -= Ax; By -= Ay;
        Cx -= Ax; Cy -= Ay;
        Dx -= Ax; Dy -= Ay;

        //  Discover the length of segment A-B.
        distAB = sqrtf(Bx*Bx + By * By);

        //  (2) Rotate the system so that point B is on the positive X axis.
        theCos = Bx / distAB;
        theSin = By / distAB;
        newX = Cx * theCos + Cy * theSin;
        Cy = Cy * theCos - Cx * theSin; Cx = newX;
        newX = Dx * theCos + Dy * theSin;
        Dy = Dy * theCos - Dx * theSin; Dx = newX;

        //  Fail if segment C-D doesn't cross line A-B.
        if ((Cy<0. && Dy<0.) || (Cy >= 0. && Dy >= 0.)) return NO;

        //  (3) Discover the position of the intersection point along line A-B.
        ABpos = Dx + (Cx - Dx)*Dy / (Dy - Cy);

        //  Fail if segment C-D crosses line A-B outside of segment A-B.
        if (ABpos<0. || ABpos>distAB) return NO;

        //  (4) Apply the discovered position to line A-B in the original coordinate system.
        float X = Ax + ABpos * theCos;
        float Y = Ay + ABpos * theSin;

        //  Success.
        return {
            true,
            0,
        { X,0,Y }
        };
    }

    IntersectionCollisionInfo segmentsCollision2(LineSegment a, LineSegment b) {
        // Copied from https://stackoverflow.com/a/1968345/848179
        float p0_x = a.start.x;
        float p0_y = a.start.z; // using p0_y and not z so i can direclty compare to the SO code
        float p1_x = a.end.x;
        float p1_y = a.end.y;
        float p2_x = b.start.x;
        float p2_y = b.start.z;
        float p3_x = b.end.x;
        float p3_y = b.end.z;

        float i_x, i_y;

        float s1_x, s1_y, s2_x, s2_y;
        s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
        s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

        float s, t;
        s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
        t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
        {
            // Collision detected
            i_x = p0_x + (t * s1_x);
            i_y = p0_y + (t * s1_y);
            return {
                true,
                t,
            { i_x, 0, i_y }
            };
        }
        return {
            false, 0.0f,{ 0.0f,0.0f,0.0f }
        };
    }

    IntersectionCollisionInfo segmentsCollisionLeMothe(LineSegment a, LineSegment b)
    {
        // Based off https://stackoverflow.com/a/1968345/848179

        LineSegment segment;
        segment.start.x = a.end.x - a.start.x;
        segment.start.z = a.end.z - a.start.z;
        segment.end.x = b.end.x - b.start.x;
        segment.end.z = b.end.z - b.start.z;

        float s = (-segment.start.z * (a.start.x - b.start.x) + segment.start.x * (a.start.z - b.start.z));
        float t = (segment.end.x * (a.start.z - a.end.z) - segment.end.z * (a.start.x - b.start.x));

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
            // Collision happened!
            t = t / (-segment.end.x*segment.start.z + segment.start.x*segment.end.z);
            return {
                true,
                t,
            { a.start.x + t * segment.start.x, 0, a.start.z + t * segment.start.z }
            };
        }
        return {
            false, 0.0f,{ 0.0f,0.0f,0.0f }
        };
    }

    double weirdCrossProduct(glm::vec3 a, glm::vec3 b) {
        return a.x*b.z - a.z*b.x;
    }

    IntersectionCollisionInfo segmentsCollision3(LineSegment l1, LineSegment l2) {

        // https://stackoverflow.com/a/14987452/848179 which itself is based off http://stackoverflow.com/a/565282/202451

        glm::vec3 p = l1.start;
        glm::vec3 q = l2.start;
        glm::vec3 r = l1.end - l1.start;
        glm::vec3 s = l2.end - l2.start;

        double s_r_crossProduct = weirdCrossProduct(r, s);
		double t = weirdCrossProduct(q - p, s) / s_r_crossProduct;
		double u = weirdCrossProduct(q - p, r) / s_r_crossProduct;

        IntersectionCollisionInfo NO = {
            false, 0.0f,{ 0.0f,0.0f,0.0f }
        };

        if (t < 0 || t > 1.0 || u < 0 || u > 1.0)
        {
            return NO;
        }
        else
        {
            return {
                true,
				static_cast<float>(t),
                p + (r * static_cast<float>(t))
            };
        }
    }

    // adapted from https://hamaluik.com/posts/swept-aabb-collision-using-minkowski-difference/
    // which was adapted from https://github.com/pgkelley4/line-segments-intersect/blob/master/js/line-segments-intersect.js
    // which was adapted from http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
    IntersectionCollisionInfo segmentsIntersection(LineSegment a, LineSegment b) {
        glm::vec3 r = a.end - a.start;
        glm::vec3 s = b.end - b.start;

        float numerator = glm::dot((b.start - a.start), r);
        float denominator = glm::dot(r, s);

        if (denominator == 0) {
            return {
                false,
                0
            };
        }
        float u = numerator / denominator;
        float t = glm::dot((b.start - a.start), s) / denominator;
        if ((t >= 0) && (t <= 1) && (u >= 0) && (u <= 1)) {
            return {
                true,
                t
            };
        }
        return {
            false,
            0
        };
    }

    CollisionInfo segmentAABBCollision(LineSegment segment, BoundingBox box, float totalTime)
    {
        // Effectively we check the segment against all four edges of the box
        float minX, minZ, maxX, maxZ;
        box = normalizeBoundingBox(box); // Just to be safe...
        minX = std::min(box.lowerCorner.x, box.upperCorner.x);
        minZ = std::min(box.lowerCorner.z, box.upperCorner.z);
        maxX = std::max(box.lowerCorner.x, box.upperCorner.x);
        maxZ = std::max(box.lowerCorner.z, box.upperCorner.z);
        glm::vec3 topLeft, bottomRight; // We are given bottom left and top right
        topLeft = { minX, 0, maxZ };
        bottomRight = { maxX, 0, minZ };

        std::vector<LineSegment> boxSides;
        boxSides.push_back({ box.lowerCorner, topLeft });
        boxSides.push_back({ topLeft, box.upperCorner });
        boxSides.push_back({ box.upperCorner, bottomRight });
        boxSides.push_back({ bottomRight, box.lowerCorner });

        IntersectionCollisionInfo collision;
        collision.collided = false;
        for (auto side : boxSides) {
            IntersectionCollisionInfo newCollision = segmentsCollision1(segment, side);
            if ((!collision.collided && newCollision.collided) || (newCollision.collided && newCollision.t < collision.t)) {
                collision = newCollision;
            }
        }

        if (collision.collided) {
            // Collision!
            return {
                true,
                collision.t*totalTime
            };
        }
        else {
            return {
                false,
                0
            };
        }
    }

    bool test()
    {
        /*
        Test suite logic follows the following diagram: https://puu.sh/zFbw6/feb4333778.png
        */
        MovingBoundingBox a;
        a.box.lowerCorner = { 1, 0, 4 };
        a.box.upperCorner = { 2, 0, 5 };
        a.position = { 1.5, 0, 4.5 };
        a.velocity = glm::vec3(4.5, 0, 4.5) - a.position;

        MovingBoundingBox b;
        b.box.lowerCorner = { 3, 0, 5.5 };
        b.box.upperCorner = { 4, 0, 6.5 };
        b.position = { 3.5, 0, 6 };
        b.velocity = glm::vec3(3.5, 0, 1.5) - b.position;

        MovingBoundingBox c;
        c.box.lowerCorner = { 1.5, 0, 2 };
        c.box.upperCorner = { 2.5, 0, 3 };
        c.position = { 2, 0, 2.5 };
        c.velocity = glm::vec3(3.5, 0, 2.5) - c.position;

        MovingBoundingBox d;
        d.box.lowerCorner = { 9,0,4 };
        d.box.upperCorner = { 10,0,5 };
        d.position = { 9.5, 0, 4.5 };
        d.velocity = glm::vec3(6.5, 0, 4.5) - d.position;

        MovingBoundingBox e;
        e.box.lowerCorner = { 6, 0, 3 };
        e.box.upperCorner = { 7, 0, 4 };
        e.position = { 6.5, 0, 3.5 };
        e.velocity = glm::vec3(9.5, 0, 1.5) - e.position;

        MovingBoundingBox f;
        f.box.lowerCorner = { 6, 0, 1 };
        f.box.upperCorner = { 7, 0, 2 };
        f.position = { 6.5, 0, 1.5 };
        f.velocity = glm::vec3(9.5, 0, 3.5) - f.position;

        std::array<MovingBoundingBox, 6> instances = { a,b,c,d,e,f };
        std::map<int, char> nameMap = {
            { 0, 'A' },{ 1, 'B' },{ 2, 'C' },{ 3, 'D' },{ 4, 'E' },{ 5, 'F' }
        };
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                if (i != j) { // No need to test against self
                    CollisionInfo collision = aabbMinkowskiCollisions(instances[i], instances[j], 1.0);
                    if (collision.collided) {
                        logger(LogLevel::INFO) << nameMap[i] << " collided with " << nameMap[j] << " at time [" << collision.time << '\n';
                    }

                }
            }
        }

        LineSegment segmentA = { { 0,0,0 },{ 1,0,1 } };
        LineSegment segmentB = { { 0,0,1 },{ 1,0,0 } };
        IntersectionCollisionInfo intersection = segmentsIntersection(segmentA, segmentB);
        IntersectionCollisionInfo intersetcionLemoth = segmentsCollisionLeMothe(segmentA, segmentB);
        IntersectionCollisionInfo intersection1 = segmentsCollision1(segmentA, segmentB);
        IntersectionCollisionInfo intersection2 = segmentsCollision2(segmentA, segmentB);
        IntersectionCollisionInfo intersection3 = segmentsCollision3(segmentA, segmentB);

        logger(LogLevel::INFO) << "Tests done \n";
        return true;
    }

}
