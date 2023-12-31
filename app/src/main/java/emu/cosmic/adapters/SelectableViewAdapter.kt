package emu.cosmic.adapters

class SelectableViewAdapter(private val defaultPos: Int) : GenericViewAdapter() {
    var selectedPos = defaultPos
    fun selectItem(position: Int) {
        if (selectedPos != position) {
            notifyItemChanged(selectedPos)
            notifyItemChanged(position)
        } else {
            notifyItemChanged(position)
        }
        selectedPos = position
    }

    fun popItem(position: Int) {
        dropItem(position)
        if (position < selectedPos)
            selectedPos--
        else if (position == selectedPos)
            selectItem(defaultPos)
        notifyItemRemoved(position)
    }

    fun insertItem(position: Int, item: GenericListContainer<*>) {
        addItem(item, position)
        notifyItemInserted(position)
    }
}