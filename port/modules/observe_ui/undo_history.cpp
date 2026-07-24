#include "undo_history.h"

namespace observe
{
  void UndoHistory::push(std::unique_ptr<UndoableAction> action)
  {
    undone_actions_.clear();
    if (actions_.size() >= kMaxUndoHistory)
      actions_.pop_front();

    actions_.push_back(std::move(action));

    for (Listener *listener : listeners_)
      listener->undoActionAdded();
  }

  void UndoHistory::undo()
  {
    if (!canUndo())
      return;

    std::unique_ptr<UndoableAction> action = std::move(actions_.back());
    actions_.pop_back();
    action->setup();
    action->undo();
    undone_actions_.push_back(std::move(action));

    for (Listener *listener : listeners_)
      listener->undoPerformed();
  }

  void UndoHistory::redo()
  {
    if (!canRedo())
      return;

    std::unique_ptr<UndoableAction> action = std::move(undone_actions_.back());
    undone_actions_.pop_back();
    action->setup();
    action->redo();
    actions_.push_back(std::move(action));

    for (Listener *listener : listeners_)
      listener->redoPerformed();
  }
}
