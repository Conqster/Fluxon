//#pragma once 
//#include "UnitTestFramework.h"
//#include "src/Core/Geometry/AABB.h"
//
//#include <glm/gtc/matrix_transform.hpp>
//
//
//TEST_SUITE("AABB class Tests")
//{
//	TEST_CASE("Default constructor creates unit cube at origin")
//	{
//		AABB box;
//
//		CHECK(box.GetCenter() == glm::vec3(0.0f));
//		CHECK(box.GetSize() == glm::vec3(1.0f));
//		CHECK(box.GetExtent() == glm::vec3(0.5f));
//	}
//
//
//	TEST_CASE("Construct from size")
//	{
//		AABB box(2.0f);
//
//		CHECK(box.GetCenter() == glm::vec3(0.0f));
//		CHECK(box.GetSize() == glm::vec3(4.0f));	//[-2,2] in each axis
//		CHECK(box.GetExtent() == glm::vec3(2.0f));
//	}
//
//
//	TEST_CASE("Construct from point")
//	{
//		glm::vec3 p(1.0f, 2.0f, 3.0f);
//		AABB box(p);
//
//		CHECK(box.GetCenter() == p);
//		CHECK(box.GetSize() == glm::vec3(0.0f));
//	}
//
//
//	TEST_CASE("Valid and invalid boxes")
//	{
//		AABB valid_box(glm::vec3(-1.0f), glm::vec3(1.0f));;
//		CHECK(valid_box.IsValid());
//
//		AABB invalid_box(glm::vec3(1.0f), glm::vec3(-1.0f));;
//		CHECK(!invalid_box.IsValid());
//	}
//
//
//	TEST_CASE("Translate AABB")
//	{
//		AABB box(glm::vec3(-1.0f), glm::vec3(1.0f));
//		box.Translate(glm::vec3(2.0f, 0.0f, 0.0f));
//		CHECK(box.GetCenter() == glm::vec3(2.0f, 0.0f, 0.0f));
//
//		//check corners
//	}
//
//
//	TEST_CASE("Expand by vector")
//	{
//		AABB box(glm::vec3(-1.0f), glm::vec3(1.0f));
//		box.Expand(glm::vec3(1.0f, 2.0f, 3.0f));
//		CHECK(box.GetSize() == glm::vec3(4.0f, 6.0f, 8.0f));
//	}
//
//	TEST_CASE("Expand by scalar")
//	{
//		AABB box(glm::vec3(-1.0f), glm::vec3(1.0f));
//		box.Expand(1.0f);
//		CHECK(box.GetSize() == glm::vec3(4.0f));
//	}
//
//	TEST_CASE("Reset sets to exterme values")
//	{
//		AABB box;
//		box.Reset();
//		CHECK(box.GetSize().x < 0.0f);
//		CHECK(!box.IsValid());
//	}
//
//
//	TEST_CASE("Get new Transformed AABB with translation")
//	{
//		AABB box(glm::vec3(-1.0f), glm::vec3(1.0f));
//
//		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f));
//		AABB moved = box.Transformed(transform);
//		
//		CHECK(moved.GetCenter() == glm::vec3(5.0f, 0.0f, 0.0f));
//	}
//
//	TEST_CASE("Encapsulate point expands box")
//	{
//		AABB box(glm::vec3(-1.0f), glm::vec3(1.0f));
//		box.Encapsulate(glm::vec3(2.0f, 0.0f, 0.0f));
//		CHECK(box.GetSize().x == 3.0f);
//	}
//
//	TEST_CASE("Encapsulate another AABB expands (overlapping)")
//	{
//		AABB box1(glm::vec3(-1.0f), glm::vec3(1.0f));
//		AABB box2(glm::vec3(0.0f), glm::vec3(2.0f));
//		box1.Encapsulate(box2);
//		CHECK(box1.GetSize() == glm::vec3(3.0f, 3.0f, 3.0f));
//	}
//
//	TEST_CASE("Encapsulate another AABB expands along axes")
//	{
//		AABB box1(glm::vec3(-1.0f), glm::vec3(1.0f));
//		//noly along z
//		AABB box2(glm::vec3(0.0f, -1.0f, -1.0f), glm::vec3(2.0f, 1.0f, 1.0f));
//		box1.Encapsulate(box2);
//		CHECK(box1.GetSize() == glm::vec3(3.0f, 2.0f, 2.0f));
//	}
//
//	TEST_CASE("Encapsulate disjoint boxes")
//	{
//		AABB box1(glm::vec3(-1.0f), glm::vec3(1.0f));
//		//noly along z
//		AABB box2(glm::vec3(3.0f), glm::vec3(4.0f));
//		box1.Encapsulate(box2);
//		CHECK(box1.GetSize() == glm::vec3(5.0f));
//	}
//
//
//	TEST_CASE("Get Corners return 8 uniques points")
//	{
//		AABB box(glm::vec3(-1.0f), glm::vec3(1.0f));
//		auto corners = box.GetCorners();
//		CHECK(corners.size() == 8);
//		CHECK(corners[0] == glm::vec3(-1.0f, -1.0f, -1.0f));
//		CHECK(corners[6] == glm::vec3(1.0f, 1.0f, 1.0f));
//	}
//}