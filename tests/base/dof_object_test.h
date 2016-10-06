#ifndef __dof_object_test_h__
#define __dof_object_test_h__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

#define DOFOBJECTTEST                           \
  CPPUNIT_TEST( testSetId );                    \
  CPPUNIT_TEST( testValidId );                  \
  CPPUNIT_TEST( testInvalidateId );             \
  CPPUNIT_TEST( testSetProcId );                \
  CPPUNIT_TEST( testValidProcId );              \
  CPPUNIT_TEST( testInvalidateProcId );         \
  CPPUNIT_TEST( testSetNSystems );              \
  CPPUNIT_TEST( testSetNVariableGroups );       \
  CPPUNIT_TEST( testManualDofCalculation );     \
  CPPUNIT_TEST( testJensEftangBug );

using namespace libMesh;

template <class DerivedClass>
class DofObjectTest {

private:
  DerivedClass * instance;

public:
  void setUp(DerivedClass * derived_instance)
  {
    instance=derived_instance;
  }

  void testSetId()
  {
    DofObject aobject(*instance);

    aobject.set_id(1);
    CPPUNIT_ASSERT_EQUAL( (dof_id_type)1 , aobject.id() );
  }

  void testValidId()
  {
    DofObject aobject(*instance);

    aobject.set_id(1);
    CPPUNIT_ASSERT( aobject.valid_id() );

    aobject.set_id(DofObject::invalid_id);
    CPPUNIT_ASSERT( !aobject.valid_id() );
  }

  void testInvalidateId()
  {
    DofObject aobject(*instance);

    aobject.set_id(1);
    aobject.invalidate_id();

    CPPUNIT_ASSERT( !aobject.valid_id() );
  }

  void testSetProcId()
  {
    DofObject aobject(*instance);

    aobject.processor_id(libMesh::global_processor_id());
    CPPUNIT_ASSERT_EQUAL( (processor_id_type)libMesh::global_processor_id() , aobject.processor_id() );
  }

  void testValidProcId()
  {
    DofObject aobject(*instance);

    aobject.processor_id(libMesh::global_processor_id());
    CPPUNIT_ASSERT(aobject.valid_processor_id());

    aobject.processor_id(DofObject::invalid_processor_id);
    CPPUNIT_ASSERT(!aobject.valid_processor_id());
  }

  void testInvalidateProcId()
  {
    DofObject aobject(*instance);

    aobject.processor_id(libMesh::global_processor_id());
    aobject.invalidate_processor_id();

    CPPUNIT_ASSERT( !aobject.valid_processor_id() );
  }

  void testSetNSystems()
  {
    DofObject aobject(*instance);

    aobject.set_n_systems (10);

    CPPUNIT_ASSERT_EQUAL( (unsigned int) 10, aobject.n_systems() );
  }

  void testSetNVariableGroups()
  {
    DofObject aobject(*instance);

    aobject.set_n_systems (2);

    std::vector<unsigned int> nvpg;

    nvpg.push_back(10);
    nvpg.push_back(20);
    nvpg.push_back(30);

    aobject.set_n_vars_per_group (0, nvpg);
    aobject.set_n_vars_per_group (1, nvpg);

    for (unsigned int s=0; s<2; s++)
      {
        CPPUNIT_ASSERT_EQUAL( (unsigned int) 60, aobject.n_vars(s) );
        CPPUNIT_ASSERT_EQUAL( (unsigned int) 3,  aobject.n_var_groups(s) );

        for (unsigned int vg=0; vg<3; vg++)
          CPPUNIT_ASSERT_EQUAL( nvpg[vg], aobject.n_vars(s,vg) );
      }
  }

  void testManualDofCalculation()
  {
    DofObject aobject(*instance);

    aobject.set_n_systems (2);

    std::vector<unsigned int> nvpg;

    nvpg.push_back(2);
    nvpg.push_back(3);

    aobject.set_n_vars_per_group (0, nvpg);
    aobject.set_n_vars_per_group (1, nvpg);

    aobject.set_n_comp_group (0, 0, 1);
    aobject.set_n_comp_group (0, 1, 3);

    aobject.set_n_comp_group (1, 0, 2);
    aobject.set_n_comp_group (1, 1, 1);

    aobject.set_vg_dof_base(0, 0, 0);
    aobject.set_vg_dof_base(0, 1, 120);

    aobject.set_vg_dof_base(1, 0, 20);
    aobject.set_vg_dof_base(1, 1, 220);

    // Make sure the first dof is sane
    CPPUNIT_ASSERT_EQUAL((dof_id_type)0, aobject.dof_number(0, 0, 0));

    // Check that we can manually index dofs of variables based on the first dof in a variable group
    // Using: id = base + var_in_vg*ncomp + comp
    CPPUNIT_ASSERT_EQUAL((dof_id_type)(aobject.vg_dof_base(0, 0) + 1*1 + 0), aobject.dof_number(0, 1, 0));

    // Another Check that we can manually index dofs of variables based on the first dof in a variable group
    // Using: id = base + var_in_vg*ncomp + comp
    CPPUNIT_ASSERT_EQUAL((dof_id_type)(aobject.vg_dof_base(0, 1) + 2*3 + 2), aobject.dof_number(0, 4, 2));

    // One More Check that we can manually index dofs of variables based on the first dof in a variable group
    // Using: id = base + var_in_vg*ncomp + comp
    CPPUNIT_ASSERT_EQUAL((dof_id_type)(aobject.vg_dof_base(1, 1) + 0*3 + 0), aobject.dof_number(1, 2, 0));
  }

  void testJensEftangBug()
  {
    // For more information on this bug, see the following email thread:
    // https://sourceforge.net/p/libmesh/mailman/libmesh-users/thread/50C8EE7C.8090405@gmail.com/
    DofObject aobject(*instance);
    dof_id_type buf0[] = {2, 8, 257, 0, 257, 96, 257, 192, 257, 0};
    aobject.set_buffer(std::vector<dof_id_type>(buf0, buf0+10));

    CPPUNIT_ASSERT_EQUAL (aobject.dof_number(0,0,0), (dof_id_type)   0);
    CPPUNIT_ASSERT_EQUAL (aobject.dof_number(0,1,0), (dof_id_type)  96);
    CPPUNIT_ASSERT_EQUAL (aobject.dof_number(0,2,0), (dof_id_type) 192);
    CPPUNIT_ASSERT_EQUAL (aobject.dof_number(1,0,0), (dof_id_type)   0);

    dof_id_type buf1[] = {2, 8, 257, 1, 257, 97, 257, 193, 257, 1};
    aobject.set_buffer(std::vector<dof_id_type>(buf1, buf1+10));

    CPPUNIT_ASSERT_EQUAL (aobject.dof_number(0,0,0), (dof_id_type)   1);
    CPPUNIT_ASSERT_EQUAL (aobject.dof_number(0,1,0), (dof_id_type)  97);
    CPPUNIT_ASSERT_EQUAL (aobject.dof_number(0,2,0), (dof_id_type) 193);
    CPPUNIT_ASSERT_EQUAL (aobject.dof_number(1,0,0), (dof_id_type)   1);
  }
};

#endif // #ifdef __dof_object_test_h__
