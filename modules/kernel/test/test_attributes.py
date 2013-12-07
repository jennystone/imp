import IMP
import IMP.test


xkey = IMP.FloatKey("x")
ykey = IMP.FloatKey("y")
zkey = IMP.FloatKey("z")
idkey = IMP.IntKey("id")
radkey = IMP.FloatKey("radius")


class Tests(IMP.test.TestCase):

    """Test particles"""

    def test_no_model(self):
        """Check access of attributes from python"""
        m = IMP.kernel.Model()
        p = IMP.kernel.Particle(m)
        ik = IMP.IntKey("hi")
        m.add_attribute(ik, p.get_index(), 1)
        self.assertEqual(m.get_attribute(ik, p.get_index()), 1)
        pisk = IMP.kernel.ParticleIndexesKey("hi")
        m.add_attribute(pisk, p.get_index(), [p.get_index()])
        self.assertEqual(m.get_attribute(pisk, p.get_index()), [p.get_index()])
        pik = IMP.kernel.ParticleIndexKey("hi")
        m.add_attribute(pik, p.get_index(), p.get_index())
        self.assertEqual(m.get_attribute(pik, p.get_index()), p.get_index())
if __name__ == '__main__':
    IMP.test.main()
