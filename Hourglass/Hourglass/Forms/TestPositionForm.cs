﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;

namespace Hourglass
{
    public partial class TestPositionForm : Form
    {

        private TransformComponent mTransformTest;
        private MeshComponent mMeshTest;
        private ColliderComponent mColliderTest;
        private BoxCollider mBoxTest;
        private PlaneCollider mPlaneTest;
        private SphereCollider mSphereTest;
        private ButtonCollider mButtonTest;
        private List<Component> mComponents;

        public TestPositionForm()
        {
            InitializeComponent();
            mComponents = new List<Component>(5);

            mTransformTest = new TransformComponent(mComponents);
            mMeshTest = new MeshComponent(mComponents);
            mColliderTest = new ColliderComponent(mComponents);
            mBoxTest = new BoxCollider(mComponents);
            mPlaneTest = new PlaneCollider(mComponents);
            mSphereTest = new SphereCollider(mComponents);
            mButtonTest = new ButtonCollider(mComponents);
            Controls.Add(mTransformTest.GetGroupbox());
            Controls.Add(mMeshTest.GetGroupbox());
            Controls.Add(mColliderTest.GetGroupbox());
            Controls.Add(mBoxTest.GetGroupbox());
            Controls.Add(mPlaneTest.GetGroupbox());
            Controls.Add(mSphereTest.GetGroupbox());
            Controls.Add(mButtonTest.GetGroupbox());
            AutoSize = false;

            Padding p = new Padding(0);
            Padding = p;
            TestPositionForm_Resize(null, null);
            ControlAdded += ReorderComponents;
            ControlRemoved += ReorderComponents;
            ActiveControl = null;
        }

        private void TestPositionForm_Click(object sender, EventArgs e)
        {
            Debug.Print("Form Clicked");
        }

        private void TestPositionForm_Resize(object sender, EventArgs e)
        {
            ReorderComponents(null, null);
        }

        private void TestPositionForm_Paint(object sender, PaintEventArgs e)
        {
        }

        private void TestPositionForm_ResizeBegin(object sender, EventArgs e)
        {
            Debug.Print("Begin Resizing");
            ActiveControl = null;
        }

        private void ReorderComponents(object sender, EventArgs e)
        {
            Point position = new Point(AutoScrollPosition.X, AutoScrollPosition.Y);
            HorizontalScroll.Value = 0;
            HorizontalScroll.Maximum = ClientSize.Width;
            Size dimensions = new Size();
            for (int i = 0; i < mComponents.Count; ++i)
            {
                GroupBox box = mComponents[i].GetGroupbox();
                box.Location = position;
                position.Y += box.Size.Height + 3;

                dimensions.Width = ClientSize.Width;
                dimensions.Height = box.Height;
                box.Size = dimensions;
            }
        }
    }
}
