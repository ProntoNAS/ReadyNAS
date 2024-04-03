import java.awt.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.awt.event.*;
import java.awt.font.*;
import java.util.*;
import javax.swing.*;

public class testswing
{
  final JFrame frame;  
  
  protected static class ButtonModelStatePrinter 
    implements javax.swing.event.ChangeListener
  {
    final String title;
    public ButtonModelStatePrinter(final String t)
    {
      title = t;
    }
    public void stateChanged(javax.swing.event.ChangeEvent e)
    {
      if (e.getSource() instanceof ButtonModel)
        {
          ButtonModel b = (ButtonModel) e.getSource();
          String state = new String();
          state += (" " + (b.isArmed() ? "+" : "-") + "armed");
          state += (" " + (b.isPressed() ? "+" : "-") + "pressed");
          state += (" " + (b.isRollover() ? "+" : "-") + "rollover");
          state += (" " + (b.isSelected() ? "+" : "-") + "selected");
          System.err.println("[" + title + "] button state: " + state);
        }
      else
        {
          System.err.println("got weird changevent from " + e.getSource());
        }
    }
  }

  public static JCheckBox mkCheckbox(String label)
  {
    JCheckBox c = new JCheckBox(label);
    c.setFont(new Font("Luxi", Font.PLAIN, 14));
    c.addChangeListener(new ButtonModelStatePrinter(label));
    return c;
  }

  public static JRadioButton mkRadio(String label)
  {
    JRadioButton c = new JRadioButton(label);
    c.setFont(new Font("Luxi", Font.PLAIN, 14));
    c.addChangeListener(new ButtonModelStatePrinter(label));
    return c;
  }

  public static JSlider mkSlider()
  {
    JSlider a = new JSlider();
    a.setPaintTrack(true);
    a.setPaintTicks(true);
    a.setMajorTickSpacing(30);
    a.setInverted(false);
    return a;
  }

  public static JList mkList(Object[] elts)
  {
    JList list = new JList(elts);
    list.setFont(new Font("Luxi", Font.PLAIN, 14));
    return list;
  }

  public static JTabbedPane mkTabs(String[] names)
  {
    JTabbedPane tabs = new JTabbedPane();
    for (int i = 0; i < names.length; ++i)
      {
        tabs.addTab(names[i], mkButton(names[i]));
      }
    return tabs;
  }
  

  public static JButton mkButton(String title)
  {
    JButton b = new JButton(title);
    b.setMargin(new Insets(5,5,5,5));
    b.setFont(new Font("Luxi", Font.PLAIN, 14));
    b.addChangeListener(new ButtonModelStatePrinter(title));
    return b;
  }

  public static JToggleButton mkToggle(String title)
  {
    JToggleButton b = new JToggleButton(title);
    b.setMargin(new Insets(5,5,5,5));
    b.setFont(new Font("Luxi", Font.PLAIN, 14));
    b.getModel().addChangeListener(new ButtonModelStatePrinter(title));
    return b;    
  }

  public static JPanel mkPanel(JComponent[] inners)
  {
    JPanel p = new JPanel();
    for (int i = 0; i < inners.length; ++i)
      {
        p.add(inners[i]);
      }
    return p;
  }

  private static class CheckCellRenderer 
    extends JCheckBox implements ListCellRenderer
  {
    public Component getListCellRendererComponent(JList list,
                                                  Object value,
                                                  int index,
                                                  boolean isSelected,
                                                  boolean cellHasFocus)
    {
      setSelected(isSelected);
      setText(value.toString());
      setFont(new Font("Luxi", Font.PLAIN, 14));
      return this;
    }
  }

  private static class LabelCellRenderer 
    extends DefaultListCellRenderer
  {
    public Component getListCellRendererComponent(JList list,
                                                  Object value,
                                                  int index,
                                                  boolean isSelected,
                                                  boolean cellHasFocus)
    {
      Component c = super.getListCellRendererComponent(list, value, index, 
                                                       isSelected, cellHasFocus);
      c.setFont(new Font("Luxi Mono", Font.PLAIN, 10));
      return c;
    }
  }

  public static JScrollBar mkScrollBar()
  {
    JScrollBar scrollbar = new JScrollBar();
    return scrollbar;
  }

  public static JPanel mkViewportBox(final JComponent inner)
  {
    final JViewport port = new JViewport();
    port.setView(inner);
    JButton left = mkButton("left");
    JButton right = mkButton("right");
    
    left.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          Point p = port.getViewPosition();
          port.setViewPosition(new Point(p.x - 10, p.y));
        }
      });

    right.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          Point p = port.getViewPosition();
          port.setViewPosition(new Point(p.x + 10, p.y));
        }
      });
 
    return mkPanel(new JComponent[] {port, left, right});
  }

  public static JScrollPane mkScrollPane(JComponent inner)
  {
    return new JScrollPane(inner,
                           JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, 
                           JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
  }

  public static JPanel mkListPanel(Object[] elts)
  {
    final DefaultListModel mod = new DefaultListModel();
    final JList list1 = new JList(mod);
    final JList list2 = new JList(mod);

    list2.setSelectionModel(list1.getSelectionModel());
    for (int i = 0; i < elts.length; ++i)
      mod.addElement(elts[i]);
    list1.setCellRenderer(new LabelCellRenderer());
    list2.setCellRenderer(new CheckCellRenderer());

    JButton add = mkButton("add element");
    add.addActionListener(new ActionListener()
      {
        int i = 0;
        public void actionPerformed(ActionEvent e)
        {
          mod.addElement("new element " + i);
          ++i;
        }
      });

    JButton del = mkButton("delete selected");
    del.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          for (int i = 0; i < mod.getSize(); ++i)
            if (list1.isSelectedIndex(i))
              mod.remove(i);
        }
      });

    return mkPanel(new JComponent[] {list1, //mkScrollPane(list1), 
                                     list2, //mkScrollPane(list2), 
                                     mkPanel(new JComponent[] {add, del})});
  }


  public static JButton mkDisposerButton(final JFrame c)
  {
    JButton close = mkButton("close");
    close.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          c.dispose();
        }
      });
    return close;
  }

  public void addCase(testcase c)
  {
    c.connectTo(this);
  }

  class testcase
    implements ActionListener
  {
    JFrame frame;
    JComponent inner;
    String name;

    testcase(String n, JComponent i)
    {
      frame = null;
      name = n;
      inner = i;
    }

    public void connectTo(testswing t)
    {
      JButton b = mkButton(name);
      b.addActionListener(this);
      t.frame.getContentPane().add(b);
    }

    public void actionPerformed(ActionEvent e)
    {
      frame = new JFrame();
      frame.getContentPane().setLayout(new BorderLayout());
      frame.getContentPane().add(inner, BorderLayout.CENTER);
      frame.getContentPane().add(mkDisposerButton(frame), BorderLayout.SOUTH);
      frame.pack();
      frame.show();
    }
  }

  public testswing ()
  {    
    frame = new JFrame ();
    Container cp = frame.getContentPane();
    cp.setLayout(new FlowLayout());
    this.addCase(new testcase("Buttons", mkPanel(new JComponent[] {mkButton("mango"), 
                                                                   mkButton("guava"),
                                                                   mkButton("lemon")})));
    this.addCase(new testcase("Toggles", mkToggle("cool and refreshing")));
    this.addCase(new testcase("Checkbox", mkCheckbox("ice cold")));
    this.addCase(new testcase("Radio", mkRadio("delicious")));
    this.addCase(new testcase("Slider", mkSlider()));
    this.addCase(new testcase("List", mkListPanel(new String[] { "hello", "this", "is", "a", "list"})));
    this.addCase(new testcase("Scrollbar", mkScrollBar()));
    this.addCase(new testcase("Viewport", mkViewportBox(mkButton("View Me!"))));
    this.addCase(new testcase("ScrollPane", mkScrollPane(mkButton("Scroll Me!"))));
    this.addCase(new testcase("TabPane", mkTabs(new String[] {"happy", "sad", "indifferent"})));

    JButton exitDisposer = mkDisposerButton(frame);
    exitDisposer.addActionListener(new ActionListener()
      {
        public void actionPerformed(ActionEvent e)
        {
          System.exit(1);
        }
      });

    frame.getContentPane().add(exitDisposer);
    frame.pack ();
    frame.show ();
  }

  public static void main(String args[])
  {
    testswing t = new testswing ();
  }

}
