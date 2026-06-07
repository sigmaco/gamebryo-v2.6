using System;
using SourceGrid2.Cells;

namespace SourceGrid2.BehaviorModels
{
	/// <summary>
	/// A behavior model with a collection of children model (SubModels). Can be used to nest a list of model.
	/// </summary>
	public class BehaviorModelGroup : IBehaviorModel
	{
		#region IBehaviorModel Members
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnContextMenuPopUp(PositionContextMenuEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnContextMenuPopUp(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnMouseDown(PositionMouseEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnMouseDown(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnMouseUp(PositionMouseEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnMouseUp(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnMouseMove(PositionMouseEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnMouseMove(e);
		}
#if !MINI
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnMouseEnter(PositionEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnMouseEnter(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnMouseLeave(PositionEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnMouseLeave(e);
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnDoubleClick(PositionEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnDoubleClick(e);
		}
#endif

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnKeyUp(PositionKeyEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnKeyUp(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnKeyDown(PositionKeyEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnKeyDown(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnKeyPress(PositionKeyPressEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnKeyPress(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnClick(PositionEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnClick(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnFocusLeaving(PositionCancelEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnFocusLeaving(e);
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnFocusLeft(PositionEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnFocusLeft(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnFocusEntering(PositionCancelEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnFocusEntering(e);
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnFocusEntered(PositionEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnFocusEntered(e);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnValueChanged(PositionEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnValueChanged(e);
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnEditStarting(PositionCancelEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnEditStarting(e);
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="e"></param>
		public virtual void OnEditEnded(PositionCancelEventArgs e)
		{
			for (int i = 0; i < m_SubModels.Count; i++)
				m_SubModels[i].OnEditEnded(e);
		}

		#endregion

		private BehaviorModelCollection m_SubModels = new BehaviorModelCollection();
		/// <summary>
		/// SubModels. Populate this list of Behavior to create a 'tree' of BehaviorModel.
		/// </summary>
		public BehaviorModelCollection SubModels
		{
			get{return m_SubModels;}
		}

		/// <summary>
		/// True if the cell can have the focus otherwise false. This method simply call BehaviorModel.CanReceiveFocus.
		/// </summary>
		public virtual bool CanReceiveFocus
		{
			get
			{
				bool ret = true;
				for (int i = 0; i < m_SubModels.Count; i++)
					ret = ret && m_SubModels[i].CanReceiveFocus;

				return ret;
			}
		}
	}
}
