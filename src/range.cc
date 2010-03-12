#include <range.h>
#include <map>
#include <font.h>
#include <utils.h>
#include <random.h>

void range::calc_notes (int octave, float start, map<char, float>& INTERVALS) {

  float start_interval = INTERVALS [intervals[0]];
  float end_interval = INTERVALS [intervals[1]];
  float start_frequency = start_interval * start;
  float end_frequency = end_interval * start;
  notes[0].set_frequency (start_frequency);
  notes[0].octave_position = octave + start_interval - 1;
  notes[1].set_frequency (end_frequency);
  notes[1].octave_position = octave + end_interval - 1;
  delta_step = notes[1].step - notes[0].step;
  delta_octave_position = notes[1].octave_position - notes[0].octave_position;
}

void range::draw_labels (int label_what) {
  
  // mark and label intervals
	glBegin (GL_LINES);
		glVertex2i (extents.left, extents.bottom);
		glVertex2i (extents.left, extents.top);
	glEnd ();
  int char_height = get_line_height ();
  draw_string (labels[0], extents.left, extents.bottom - char_height);
  draw_string (labels[0], extents.left, extents.top + char_height);
  if (label_what == BOTH) {
    draw_string (labels[1], extents.right, extents.bottom - char_height);
    draw_string (labels[1], extents.right, extents.top + char_height);
    glBegin (GL_LINES);
      glVertex2i (extents.right, extents.bottom);
      glVertex2i (extents.right, extents.top);
    glEnd ();
  }

}

void range::set_extents (int left, int bottom, int right, int top) {

  extents (left, bottom, right, top);

}
